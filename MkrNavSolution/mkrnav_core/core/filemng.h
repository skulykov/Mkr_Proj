#ifndef SK_FILE_MNG_H
#define SK_FILE_MNG_H
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#if SK_BUILD_FOR_ANDROID
#include <sys/mman.h>
#endif
#include <fcntl.h>

#include "../ogl/Sk_Types.h"
#include "dbg.h"

#pragma region BufferStream
class BufferStream
{
public:
    BufferStream()
	{
		this->buffer = NULL;
		this->currentPosition = 0;
		this->bufferLength = 0;
	}
    BufferStream(void* buffer, U32 bufferLength)
	{
		this->buffer = (U8*)buffer;
		currentPosition = 0;
		this->bufferLength=bufferLength;
	}

    void setBuffer(void* buffer, U32 bufferLength)
	{
		this->buffer = (U8*)buffer;
		currentPosition = 0;
		this->bufferLength=bufferLength;
	}
    bool read(void* outputBuffer, U32 bytesToRead){
		for(int i=0;i<bytesToRead;i++) {
			if(!get(((U8*)outputBuffer)[i])) {
				return false;
			}
		}
		return true;
	}
    bool get(U8& byte){
		if(currentPosition<bufferLength) {
			byte = buffer[currentPosition];
			currentPosition++;
			return true;
		}

		return false;
	}
    bool setPosition(U32 position){
		if(position < bufferLength) {
			currentPosition = position;
			return true;
		}
		return false;
	}
    U32 getPosition(){
		return currentPosition;
	}
    void resetPosition(){
		currentPosition = 0;
	}

private:
    U8* buffer;
    U32 currentPosition;
    U32 bufferLength;
};
#pragma endregion

#pragma region SK_FILE_PATH

#ifdef IPHONE
#define HOME_PREFIX ""
#else
#define HOME_PREFIX ".fcnav"
#endif


void sk_path_set  (const char *name, const char *path);

const char *sk_path_first (const char *name);
const char *sk_path_next  (const char *name, const char *current);

const char *sk_path_last     (const char *name);
const char *sk_path_previous (const char *name, const char *current);

const char *sk_path_preferred (const char *name);

const char *sk_path_user    (void);
const char *sk_path_trips   (void);

char *sk_path_join (const char *path, const char *name);
char *sk_path_parent (const char *path, const char *name);
void sk_path_format (char *buffer, int buffer_size, const char *path, const char *name);

void sk_path_create (const char *path);

char **sk_path_list (const char *path, const char *extension);
void   sk_path_list_free (char **list);

void sk_path_free (const char *path);

const char *sk_path_search_icon (const char *name);

char *sk_path_skip_directories (const char *name);
char *sk_path_remove_extension (const char *name);

int sk_path_is_full_path (const char *name);

const char *sk_path_temporary (void);

int sk_path_is_directory (const char *name);

const char *sk_path_gps( void );

const char *sk_path_images( void );
const char *sk_path_voices( void );
const char *sk_path_tts( void );

const char *sk_path_downloads( void );

const char *sk_path_config( void );

#ifdef IPHONE
const char *sk_path_bundle (void);
#else
const char *sk_path_sdcard (void);
#endif //IPHONE

const char *sk_path_debug( void );
#pragma endregion

#pragma region SK_FILE


#if !defined (ANDROID) && !defined (__SYMBIAN32__)

#include <windows.h>
#include <stdio.h>

typedef HANDLE SK_FILE; /* WIN32 style. */
#define SK_FILE_IS_VALID(f) (f != INVALID_HANDLE_VALUE)
#define SK_INVALID_FILE INVALID_HANDLE_VALUE

#elif defined J2ME

#include <stdio.h>

typedef FILE* SK_FILE;
#define SK_FILE_IS_VALID(f) (f != NULL)
#define SK_INVALID_FILE NULL

#elif defined __SYMBIAN32__

#include <stdio.h>

typedef void* SK_FILE;
#define SK_FILE_IS_VALID(f) (f != NULL)
#define SK_INVALID_FILE NULL

#else

#include <stdio.h>

typedef int SK_FILE; /* UNIX style. */
#define SK_FILE_IS_VALID(f) (f != (SK_FILE)-1)
#define SK_INVALID_FILE -1

#endif

struct Sk_FileContextStructure {

   int   fd;
   void *base;
   int   size;
};
typedef enum {
	SK_SEEK_START,
	SK_SEEK_CURR,
	SK_SEEK_END
} Sk_SeekWhence; 

SK_FILE sk_file_open  (const char *name, const char *mode);
int   sk_file_read  (SK_FILE file, void *data, int size);
int   sk_file_write (SK_FILE file, const void *data, int length);
int   sk_file_seek  (SK_FILE file, int offset, Sk_SeekWhence whence); 
void  sk_file_close (SK_FILE file);

void  sk_file_remove (const char *path, const char *name);
int   sk_file_exists (const char *path, const char *name);
int   sk_file_length (const char *path, const char *name);

void  sk_file_save (const char *path, const char *name,
                         void *data, int length);

void sk_file_append (const char *path, const char *name,
                          void *data, int length);

int sk_file_rename (const char *old_name, const char *new_name);

int sk_file_truncate (const char *path, const char *name,
                           int length);

FILE *sk_file_fopen (const char *path, const char *name, const char *mode);


/* The following file operations hide the OS file mapping primitives. */

struct Sk_FileContextStructure;
typedef struct Sk_FileContextStructure *Sk_FileContext;

const char *sk_file_map (const char *set,
                              const char *name,
                              const char *sequence,
                              const char *mode,
                              Sk_FileContext *file);

void *sk_file_base (Sk_FileContext file);
int   sk_file_size (Sk_FileContext file);

int  sk_file_sync (Sk_FileContext file);
void sk_file_unmap (Sk_FileContext *file);

const char *sk_file_unique (const char *base);

int sk_file_free_space (const char *path);
void sk_file_rmdir( const char *path, const char *name );
#pragma endregion



#endif  // SK_FILE_MNG_H