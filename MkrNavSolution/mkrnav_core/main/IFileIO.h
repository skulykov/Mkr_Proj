#ifndef FILEIO_H
#define FILEIO_H
#include "../ogl/Sk_Types.h"


class AndroidFileIO;

typedef U8 EngineFileHandle;


class IFileIO
{
public:
    IFileIO() {}
    virtual ~IFileIO(){}

    virtual bool Initialize()=0;
    virtual bool Release() = 0;

    virtual U32 GetAssetSize(const char* path)=0;
    virtual U32 GetFileSize(const char* path)=0;

    virtual bool OpenFile(const char* path, const char* mode, EngineFileHandle& fileHandle)=0;
    virtual void CloseFile(EngineFileHandle fileHandle)=0;
    virtual bool ReadFromFile(EngineFileHandle fileHandle, void* buffer, U32 bufferSize)=0;
    virtual bool ReadFromFile(EngineFileHandle fileHandle, void* buffer, U32 bufferSize, U32& bytesRead)=0;
    virtual bool WriteToFile(EngineFileHandle fileHandle, void*buffer, U32 bufferSize)=0;
    virtual bool WriteToFile(EngineFileHandle fileHandle, void*buffer, U32 bufferSize, U32& bytesWrite)=0;

    virtual bool ReadAsset(const char* path, void* buffer, U32 bufferSize)=0;
    virtual bool ReadAsset(const char* path, void* buffer, U32 bufferSize, U32& bytesRead)=0;

    virtual bool ReadFile(const char* path, void* buffer, U32 bufferSize)=0;
    virtual bool ReadFile(const char* path, void* buffer, U32 bufferSize, U32& bytesRead)=0;

    virtual bool WriteFile(const char* path, void* buffer, U32 bufferSize)=0;
    virtual bool WriteFile(const char* path, void* buffer, U32 bufferSize, U32& byteWrite)=0;


    virtual bool Read(const char* path, void*buffer, U32 bufferSize)=0;
    virtual bool Read(const char* path, void* buffer, U32 bufferSize, U32& bytesRead)=0;
    virtual U32 GetSize(const char* path)=0;


    static IFileIO* get(){return singleton;}

protected:
    static IFileIO* singleton;
};

#endif // FILEIO_H
