#if SK_BUILD_FOR_ANDROID
#ifndef ANDROIDFILEIO_H
#define ANDROIDFILEIO_H
#include "IFileIO.h"
#include <android/asset_manager.h>
#include <stdio.h>

class AndroidFileIO : public IFileIO
{
public:
    explicit AndroidFileIO(AAssetManager* assetManager);
    ~AndroidFileIO();

    bool Initialize();
    bool Release();

    U32 GetFileSize(const char *path);
    U32 GetAssetSize(const char *path);

    bool OpenFile(const char* path, const char* mode, EngineFileHandle& fileHandle);
    void CloseFile(EngineFileHandle fileHandle);

    bool ReadFromFile(EngineFileHandle fileHandle, void* buffer, U32 bufferSize);
    bool ReadFromFile(EngineFileHandle fileHandle, void* buffer, U32 bufferSize, U32& bytesRead);
    bool WriteToFile(EngineFileHandle fileHandle, void*buffer, U32 bufferSize);
    bool WriteToFile(EngineFileHandle fileHandle, void*buffer, U32 bufferSize, U32& bytesWrite);

    bool ReadAsset(const char* path, void* buffer, U32 bufferSize);
    bool ReadAsset(const char* path, void* buffer, U32 bufferSize, U32& bytesRead);

    bool ReadFile(const char* path, void* buffer, U32 bufferSize);
    bool ReadFile(const char* path, void* buffer, U32 bufferSize, U32& bytesRead);

    bool WriteFile(const char* path, void* buffer, U32 bufferSize);
    bool WriteFile(const char* path, void* buffer, U32 bufferSize, U32& byteWrite);

    bool Read(const char* path, void*buffer, U32 bufferSize);
    bool Read(const char* path, void* buffer, U32 bufferSize, U32& bytesRead);
    U32 GetSize(const char* path);

private:
    AAssetManager* assetManager;
    FILE* fileHandles[256];
    U8 numHandles;
};

#endif // ANDROIDFILEIO_H
#endif
