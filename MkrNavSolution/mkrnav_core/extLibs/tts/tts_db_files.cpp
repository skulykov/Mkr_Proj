/* tts_db_files.c - implementation of the Text To Speech (TTS) files based database layer
 */

#include <stdlib.h>
#include <string.h>
#include "tts_db_files.h"
#include "filemng.h"


//======================== Local defines ========================

//======================== Local types ========================

//======================== Globals ========================


//======================== Local Declarations ========================
static BOOL _check_parent( const char* path, BOOL is_create );

/*
 ******************************************************************************
 */
BOOL tts_db_files_store( const TtsPath* db_path, const TtsData* db_data )
{
   SK_FILE file;
   BOOL result = FALSE;
   const char* path;
   if ( db_path == NULL )
   {
      sk_log( SK_ERROR, TTS_LOG_STR( "Path was not supplied!" ) );
      return FALSE;
   }

   if ( db_data && db_data->data )
   {
      path = db_path->path;
      // Save the resource to the file
      sk_log( SK_INFO, "Storing file at %s", path );

      file = sk_file_open( path, "w" );
      if ( !SK_FILE_IS_VALID( file ) )
      {
         // Last chance - try to create the parent
         _check_parent( path, TRUE );
         file = sk_file_open( path, "w" );
      }

      if ( SK_FILE_IS_VALID( file ) )
      {
         sk_file_write( file, db_data->data, db_data->data_size );
         sk_file_close( file );
         result = TRUE;
      }
      else
      {
         result = FALSE;
         sk_log( SK_ERROR, "Error opening file: %s", path );
      }
   }

   return result;
}

/*
 ******************************************************************************
 */
void tts_db_files_remove( const TtsPath* db_path )
{
   if ( db_path == NULL )
   {
      sk_log( SK_ERROR, TTS_LOG_STR( "Path was not supplied!" ) );
      return;
   }

   sk_file_remove( db_path->path, NULL );
}

/*
 ******************************************************************************
 */
BOOL tts_db_files_get( const TtsPath* db_path, TtsData* db_data )
{
   BOOL result = FALSE;

   SK_FILE file;

   if ( db_path == NULL )
   {
      sk_log( SK_ERROR, TTS_LOG_STR( "Path was not supplied!" ) );
      return FALSE;
   }


   if ( db_data && ( file = sk_file_open( db_path->path, "r" ) ) )
   {
     db_data->data_size = sk_file_length( db_path->path, NULL );
     db_data->data = malloc( db_data->data_size );
     sk_check_allocated( db_data->data );
     sk_file_read( file, db_data->data, db_data->data_size );
     result = TRUE;
   }

   return result;
}

/*
 ******************************************************************************
 */
BOOL tts_db_files_exists( const TtsPath* db_path )
{
   BOOL result;

   if ( db_path == NULL )
   {
      sk_log( SK_ERROR, TTS_LOG_STR( "Path was not supplied!" ) );
      return FALSE;
   }

   result = sk_file_exists( db_path->path, NULL );

   return result;
}


/*
 ******************************************************************************
 * Auxiliary
 * Checks if parent directory exists
 * Params: is_create - TRUE create the parent if necessary
 */
static BOOL _check_parent( const char* path, BOOL is_create )
{
   const char* parent = sk_path_parent( path, NULL );
   BOOL result = sk_file_exists( parent, NULL );

   if ( !result )
   {
      sk_log( SK_WARNING, "Path %s doesn't exist!. Creating: %d", parent, is_create );
      if ( is_create )
      {
         sk_path_create( parent );
         result = TRUE;
      }
   }
   sk_path_free( parent );

   return result;
}


