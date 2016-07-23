#ifndef INCLUDE__SKSOUND__H
#define INCLUDE__SKSOUND__H
#include <stdlib.h>
#include <string.h>
#include "mkrnav.h"
#include "filemng.h"
#define MAX_SOUND_LIST 20

#ifdef __SYMBIAN32__
//TODO temp
#define MAX_SOUND_NAME 300
#elif defined(IPHONE) || defined(ANDROID)
#define MAX_SOUND_NAME 512
#else
#define MAX_SOUND_NAME 100
#endif

#define SOUND_LIST_NO_FREE 0x1
#define SOUND_LIST_BUFFERS 0x2

#define SND_LIST_ERR_GENERAL     -1
#define SND_LIST_ERR_LIST_FULL   -2
#define SND_LIST_ERR_NO_FILE     -3


#ifndef J2ME
struct sk_sound_list_t {
   int flags;
   int count;
   char list[MAX_SOUND_LIST][MAX_SOUND_NAME];
   void* buf_list[MAX_SOUND_LIST];
   size_t buf_list_sizes[MAX_SOUND_LIST];
};

typedef struct sk_sound_list_t *Sk_SoundList;

#else

typedef int Sk_SoundList;
#endif

/* Defined in C file */
#define __SND_VOLUME_LVLS_COUNT__ (4)

extern const int SND_VOLUME_LVLS_COUNT;// = __SND_VOLUME_LVLS_COUNT__;
extern const int SND_VOLUME_LVLS[];// = {0, 1, 2, 3};
extern const char* SND_VOLUME_LVLS_LABELS[__SND_VOLUME_LVLS_COUNT__];
extern const char* SND_DEFAULT_VOLUME_LVL;// = "2";

struct roadmap_sound_st;
typedef struct roadmap_sound_st *Sk_Sound;
void MkRlSoundManager_PlayFile( const char* aFileName );
class Sk_CSound
{
	
public:
	static BOOL sgInitialized;

	static void sk_sound_initialize ()
	{

	   int curLvl;
	   char sound_dir[MAX_SOUND_NAME];

	   // Initialize the volume labels for GUI
	   SND_VOLUME_LVLS_LABELS[0] = Mkr::sk_lang_get( "Silent" );
	   SND_VOLUME_LVLS_LABELS[1] = Mkr::sk_lang_get( "Low" );
	   SND_VOLUME_LVLS_LABELS[2] = Mkr::sk_lang_get( "Medium" );
	   SND_VOLUME_LVLS_LABELS[3] = Mkr::sk_lang_get( "High" );

	//   // Set current volume from the configuration
	//   sk_config_declare("user", &Sk_ConfigVolControl, SND_DEFAULT_VOLUME_LVL, NULL );
	//   curLvl = sk_config_get_integer( &Sk_ConfigVolControl );
	////   FreeMapNativeManager_SetVolume( curLvl, SND_VOLUME_LVLS[0], SND_VOLUME_LVLS[SND_VOLUME_LVLS_COUNT-1] );

	//   // Preload the sound resources
	//   snprintf( sound_dir, sizeof( sound_dir ), "%s//%s//%s",
	//				sk_path_downloads(), "sound", sk_prompts_get_name() );
	//   FreeMapNativeSoundManager_LoadSoundData( sound_dir );

	   sgInitialized = TRUE;
	   // Log the operation
	   sk_log( SK_DEBUG, "Current volume initialized to level : %d.", curLvl );
	}
	
	static void sk_sound_shutdown ()
	{
	//  CSk_NativeSound::FreeInstance();
	}

	static  Sk_SoundList sk_sound_list_create ( int flags )
	{
	  Sk_SoundList list =
			(Sk_SoundList) calloc ( 1, sizeof( struct sk_sound_list_t ) );

	  list->flags = flags;

	  return list;
	}

	static  int sk_sound_list_add ( Sk_SoundList list, const char *name )
	{
	  const char* full_name;
	  if ( list->count == MAX_SOUND_LIST ) return SND_LIST_ERR_LIST_FULL;

	  full_name = get_full_name( name );

	  if ( !sk_file_exists( full_name, NULL ) )
	  {
		 sk_log( SK_ERROR, "File %s doesn't exist! Cannot add to the list.", full_name );
		 return SND_LIST_ERR_NO_FILE;
	  }

	  strncpy (list->list[list->count], name, sizeof(list->list[0]));
	  list->list[list->count][sizeof(list->list[0])-1] = '\0';
	  list->count++;

	  return list->count - 1;
	}

	static int sk_sound_list_add_buf (Sk_SoundList list, void* buf, size_t size )
	{
	   char path[512];
	   int file_num = list->count;
	   SK_FILE file;

	   if (list->count == MAX_SOUND_LIST) return SND_LIST_ERR_LIST_FULL;

	   list->buf_list[list->count] = buf;
	   list->buf_list_sizes[list->count] = size;


	   /*
		* Temporary solution - write the buffer to the file for further playing
		* AGA
		*/
	   sprintf( path, "%s/tmp/%d", sk_path_tts(), file_num );
	   if ( file_num == 0 )
	   {
		  sk_path_create( sk_path_parent( path, NULL ) );
	   }

	   file = sk_file_open( path, "w" );
	   sk_file_write( file, buf, size );
	   sk_file_close( file );

	   strncpy_safe( list->list[list->count], path, 512 );


	   list->count++;

	   return list->count - 1;
	}


	/*************************************************************************************************
	 * sk_sound_play_list()
	 * Playing the sound files in the list by calling the JNI layer
	 * FreeMapNativeSoundManager_PlaySoundFile utility
	 */
	static int sk_sound_play_list( const Sk_SoundList list )
	{
		if ( sgInitialized )
		{
			int listSize = sk_sound_list_count( list );
			const char *full_name;
			int i;

			for( i = 0; i < sk_sound_list_count(list); i++ )
			{
				const char *name = sk_sound_list_get ( list, i );
				// !!!! Resources are not supported !!!!
				// Sk_Sound sound = (Sk_Sound) sk_res_get (RES_SOUND, RES_NOCREATE, name);

				// AGA NOTE :: TODO :: Temporary solution. All the resources are extracted now.
				// The unnecessary sounds should be cleared/not extracted (?)
				 if ( (list->flags & SOUND_LIST_BUFFERS) == 0 )
				 {
					 full_name = get_full_name( name );
					 // Calling the JNI layer
					 MkRlSoundManager_PlayFile( full_name );
				 }
				 else
				 {
					/*
					 * Temporary solution - write the buffer to the file for further playing
					 * AGA
					 */
		//            FreeMapNativeSoundManager_PlayFile( sk_sound_list_get ( list, i ) );
				 //			   FreeMapNativeSoundManager_PlayBuffer( list->buf_list[i], list->buf_list_sizes[i] );
		//            free( list->buf_list[i] );
				 }
		}
		}
		// Deallocation
		if ( (list->flags & SOUND_LIST_NO_FREE) == 0x0 )
		{
			free (list);
		}
		return 0;
	}




	static  Sk_Sound sk_sound_load (const char *path, const char *file, int *mem)
	{

	   char *full_name = sk_path_join (path, file);
	   const char *seq;
	   Sk_FileContext sound;
	   char sound_filename[MAX_SOUND_NAME];

	   return NULL;

	   snprintf( sound_filename, sizeof(sound_filename), "%s.mp3", full_name);

	   seq = sk_file_map (NULL, sound_filename, NULL, "r", &sound);

	   sk_path_free (full_name);

	   if (seq == NULL) {
		  *mem = 0;
		  return NULL;
	   }

	   *mem = sk_file_size (sound);

	   return (Sk_Sound) sound;
	}

	static int sk_sound_free (Sk_Sound sound)
	{

	   sk_file_unmap ((Sk_FileContext*)(&sound));

	   return 0;
	}

	static int sk_sound_list_count (const Sk_SoundList list) {

	   return list->count;
	}

	static const char *sk_sound_list_get (const Sk_SoundList list, int i)
	{

	   if (i >= MAX_SOUND_LIST) return NULL;

	   return list->list[i];
	}

	static void sk_sound_list_free (Sk_SoundList list) {

	   free(list);
	}

	static int sk_sound_play_file (const char *file_name) {
	   return -1;
	}

	static int sk_sound_record (const char *file_name, int seconds) {   
	   return -1;//WazeSoundRecorder_Start( file_name, seconds*1000 );
	}

	static void sk_sound_stop_recording (void){
	   //WazeSoundRecorder_Stop();
	}

	/***********************************************************
	 *	Name 		: sk_sound_set_volume
	 *	Purpose 	: Sets the user volume setting to the native sound object
	 * 					with configuration update
	 */
	static void sk_sound_set_volume ( int volLvl )
	{
		// Update the device
	//	FreeMapNativeManager_SetVolume( volLvl, SND_VOLUME_LVLS[0], SND_VOLUME_LVLS[SND_VOLUME_LVLS_COUNT-1] );

		// Update the configuration
		//sk_config_set_integer( &Sk_ConfigVolControl, volLvl );

		//// Log the operation
		//sk_log( SK_DEBUG, "Current volume is set to level : %d.", volLvl );

	}


	static const char* get_full_name( const char* name )
	{
	   static char full_name[256];
	   const char *suffix = "";

	   if ( !strchr( name, '.' ) )
	   {
		  suffix = ".bin";
	   }

	   if ( sk_path_is_full_path( name ) )
	   {
		  strncpy_safe( full_name, name, sizeof( full_name ) );
	   }
	   else
	   {
		  snprintf( full_name, sizeof( full_name ), "%s//%s//%s//%s%s",
				sk_path_downloads(), "sound", /*sk_prompts_get_name()*/"MkrNav", name, suffix );
	   }
	   return full_name;
	}

};



#endif //INCLUDE__SKSOUND__H