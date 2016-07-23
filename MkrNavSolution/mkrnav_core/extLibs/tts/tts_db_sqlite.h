/* tts_db_sqlite.h - The interface for the text to speech sqlite database management module
 
 */


#ifndef INCLUDE__TTS_DB_SQLITE__H
#define INCLUDE__TTS_DB_SQLITE__H
#ifdef __cplusplus
extern "C" {
#endif

#include "mkrnav.h"
#include "tts_db.h"

/*
 * Stores the voice data to the database
 *
 * Params: entry - the TTS db entry description
 *         storage_type - marks what storage is used to save data bytes
 *         db_data - the pointer to the audio data structure
 *         db_path - the full path to the audio file
 */
BOOL tts_db_sqlite_store( const TtsDbEntry* entry, TtsDbDataStorageType storage_type, const TtsData* db_data, const TtsPath* db_path );

/*
 * Removes the voice from the database in one transaction
 *
 * Params: entry - the TTS db entry description
 *
 */
BOOL tts_db_sqlite_remove( const TtsDbEntry* entry );

/*
 * Loads the voice from the database
 *
 * Params: entry - the TTS db entry description
 *       : [in] entry - the database entry description
 *       : [out] storage_type - the data storage type
 *       : [out] db_data - the pointer to the data pointer to update
 *       : [out] path - the array to store the path into
 */
BOOL tts_db_sqlite_get( const TtsDbEntry* entry, TtsDbDataStorageType* storage_type, TtsData *db_data, TtsPath* db_path );

/*
 * Loads the record info
 *
 * Params: entry - the TTS db entry description
 *       : [out] text_type - the pointer to the type of the text (can be NULL)
 *       : [out] storage_type - the storage type of data (can be NULL)
 *       : [out] db_path - the path to store into (can be NULL)
 */
BOOL tts_db_sqlite_get_info( const TtsDbEntry* entry, TtsTextType* text_type, TtsDbDataStorageType* storage_type, TtsPath* db_path );


/*
 * Removes voices table
 *
 * Params: voice_id
 *
 */
BOOL tts_db_sqlite_destroy_voice( const char* voice_id );

/*
 * Checks if the synthesized text is in the db
 *
 * Params: entry - the TTS db entry description
 * Returns true - if exists
 */
BOOL tts_db_sqlite_exists( const TtsDbEntry* entry );
/*
 * Destroys the entire database
 *
 * Params: void
 *
 */
void tts_db_sqlite_destroy( void );


#ifdef __cplusplus
}
#endif
#endif // INCLUDE__TTS_DB_SQLITE__H
