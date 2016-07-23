#if SK_BUILD_FOR_ANDROID
#include "AndroidFileIO.h"
#include <sys/stat.h>
#include "mkrnav.h"
#include <stdlib.h>
#include <stdio.h>




IFileIO* IFileIO::singleton=NULL;

AndroidFileIO::AndroidFileIO(AAssetManager *assetManager) : IFileIO() {
    this->assetManager = assetManager;
}

AndroidFileIO::~AndroidFileIO() {

}

bool AndroidFileIO::Initialize() {
  
	if(singleton!=NULL)
	{
		//sk_log (SK_INFO, "FileIO system already initialized");
		return true;
	}

    singleton = this;

    for(int i=0;i<256;i++)
        fileHandles[i]=NULL;

    numHandles=0;

    //sk_log (SK_INFO, "Android FileIO system initialized");
    return true;
}

bool AndroidFileIO::Release() {
    if(numHandles!=0);
    for(int i=0;i<256;i++) {
        if(fileHandles[i]!=NULL) {
            fclose(fileHandles[i]);
            fileHandles[i] = NULL;
        }
    }
    singleton = NULL;
   // sk_log (SK_INFO, "Android FileIO system released");
    return true;
}

bool AndroidFileIO::OpenFile(const char *path, const char *mode, EngineFileHandle &fileHandle) {
    if(numHandles>=256) return false;


    char prefix[128];
    strcpy(prefix, "/sdcard/");
    strcat(prefix, path);

    for(int i=0;i<256;i++) {
        if(fileHandles[i]==NULL) {
            fileHandle = i;
            numHandles++;
            break;
        }
    }

    fileHandles[fileHandle] = fopen(prefix, mode);
    if(fileHandles[fileHandle]) {
        return 1;
    }

    fileHandles[fileHandle] = NULL;
    numHandles--;
    return -1;
}

void AndroidFileIO::CloseFile(EngineFileHandle fileHandle) {
    if(fileHandle>=0 && fileHandle<256) {
        fclose(fileHandles[fileHandle]);
        fileHandles[fileHandle]=NULL;
        numHandles--;
    }
}

bool AndroidFileIO::ReadFromFile(EngineFileHandle fileHandle, void *buffer, U32 bufferSize) {
    if(fileHandle>=0 && fileHandle<256) {
        size_t read = fread(buffer, 1, bufferSize, fileHandles[fileHandle]);

        int error = ferror(fileHandles[fileHandle]);
        if(0 == error) {
            return true;
        }
        return false;
    }
    return false;
}

bool AndroidFileIO::ReadFromFile(EngineFileHandle fileHandle, void *buffer, U32 bufferSize, U32& bytesRead) {
    if(fileHandle>=0 && fileHandle<256) {
        size_t read = fread(buffer, 1, bufferSize, fileHandles[fileHandle]);

        int error = ferror(fileHandles[fileHandle]);
        if(0 == error) {
            bytesRead = read;
            return true;
        }
        return false;
    }
    return false;
}


bool AndroidFileIO::WriteToFile(EngineFileHandle fileHandle, void *buffer, U32 bufferSize) {
    if(fileHandle>=0 && fileHandle<256) {
        size_t read = fwrite(buffer, 1, bufferSize, fileHandles[fileHandle]);

        int error = ferror(fileHandles[fileHandle]);
        if(0 == error) {
            return true;
        }
        return false;
    }
    return false;
}

bool AndroidFileIO::WriteToFile(EngineFileHandle fileHandle, void *buffer, U32 bufferSize, U32& bytesWrite) {
    if(fileHandle>=0 && fileHandle<256) {
        size_t read = fread(buffer, 1, bufferSize, fileHandles[fileHandle]);

        int error = ferror(fileHandles[fileHandle]);
        if(0 == error) {
            bytesWrite = read;
            return true;
        }
        return false;
    }
    return false;
}

U32 AndroidFileIO::GetAssetSize(const char *path) {
    AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);

    if(asset == NULL)
        return 0;


    int len = AAsset_getLength(asset);
    AAsset_close(asset);
    return len;
}

U32 AndroidFileIO::GetFileSize(const char *path) {
    struct stat st;
    size_t fileSize=0;
    if(stat(path,&st) == 0 ){
         fileSize = st.st_size;
    }
    return fileSize;
}


bool AndroidFileIO::ReadAsset(const char *path, void* buffer, U32 bufferSize) {
    AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);

    if(asset == NULL)
        return false;

    AAsset_read(asset, buffer, bufferSize);

    AAsset_close(asset);

    return true;
}

bool AndroidFileIO::ReadAsset(const char *path, void* buffer, U32 bufferSize, U32& bytesRead) {
    AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);

    if(asset == NULL)
        return false;

    bytesRead = AAsset_read(asset, buffer, bufferSize);

    AAsset_close(asset);

    return true;
}


bool AndroidFileIO::ReadFile(const char *path, void *buffer, U32 bufferSize) {
    FILE* pf = fopen(path, "r");
    if(pf == NULL)
        return false;

    size_t read = fread(buffer, 1, bufferSize, pf);

    int error = ferror(pf);
    fclose(pf);

    if(0 == error) {
        return true;
    }
    return false;
}

bool AndroidFileIO::ReadFile(const char *path, void *buffer, U32 bufferSize, U32 &bytesRead) {
    FILE* pf = fopen(path, "r");
    if(pf == NULL)
        return false;


    size_t read = fread(buffer, 1, bufferSize, pf);

    int error = ferror(pf);
    fclose(pf);

    if(0 == error) {
        bytesRead = read;
        return true;
    }
    return false;
}


bool AndroidFileIO::WriteFile(const char *path, void *buffer, U32 bufferSize) {
    FILE* pf = fopen(path, "w");
    if(pf == NULL)
        return false;


    size_t write = fwrite(buffer, 1, bufferSize, pf);

    int error = ferror(pf);
    fclose(pf);

    if(0 == error) {
        return true;
    }
    return false;
}

bool AndroidFileIO::WriteFile(const char *path, void *buffer, U32 bufferSize, U32 &byteWrite) {
    FILE* pf = fopen(path, "w");
    if(pf == NULL)
        return false;


    size_t write = fwrite(buffer, 1, bufferSize, pf);

    int error = ferror(pf);
    fclose(pf);

    if(0 == error) {
        byteWrite = write;
        return true;
    }
    return false;
}



U32 AndroidFileIO::GetSize(const char *path) {
    if(path[0] == ':') {
        const char* assetPath = &path[1];
        return GetAssetSize(assetPath);
    }
    else {
        char prefix[128];
        strcpy(prefix, "/sdcard/");
        strcat(prefix, path);

        return GetFileSize(prefix);
    }
}

bool AndroidFileIO::Read(const char *path, void *buffer, U32 bufferSize) {
    if(path[0] == ':') {
        const char* assetPath = &path[1];
        return ReadAsset(assetPath, buffer, bufferSize);
    }
    else {
        char prefix[128];
        strcpy(prefix, "/sdcard/");
        strcat(prefix, path);
        return ReadFile(prefix, buffer, bufferSize);
    }
}

bool AndroidFileIO::Read(const char *path, void *buffer, U32 bufferSize, U32 &bytesRead) {
    if(path[0] == ':') {
        const char* assetPath = &path[1];
        return ReadAsset(assetPath, buffer, bufferSize, bytesRead);
    }
    else {
        char prefix[128];
        strcpy(prefix, "/sdcard/");
        strcat(prefix, path);
        return ReadFile(prefix, buffer, bufferSize, bytesRead);
    }
}
#endif

