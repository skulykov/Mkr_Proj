/* tts_queue.c - Text To Speech (TTS) interface layer implementation
 *                       Queue management
 *
 */


#include "mkrnav.h"
#include "tts_queue.h"
#include <string.h>
#include "sk_hash.h"

//======================== Local defines ========================


//======================== Local types ========================
typedef struct {
   int busy;
   int next;
   int status;
   void* context;
   const char* key;
   time_t request_time;
} TtsQueueEntry;


//======================== Globals ========================

/*
 * Queue attributes
 */
static TtsQueueEntry sgTtsQueue[TTS_QUEUE_SIZE];
static int sgTtsQueueHead = -1;
static int sgTtsQueueTail = 0;
const TtsQueueEntry sgcTtsQueueEntryInitializer = { 0, -1, TTS_QUEUE_STATUS_UNDEFINED, NULL, NULL, 0 };

static Sk_Hash* sgTtsQueueHash = NULL;

//======================== Local Declarations ========================

/*
 ******************************************************************************
 */
void tts_queue_init( void )
{
   int i;
   for ( i = 0; i < TTS_QUEUE_SIZE; ++i )
   {
      sgTtsQueue[i] = sgcTtsQueueEntryInitializer;
   }
   sgTtsQueueHead = -1;
   // Hash initialization
   sgTtsQueueHash = sk_hash_new( "TTS QUEUE", TTS_QUEUE_SIZE );
}

/*
 ******************************************************************************
 */
void tts_queue_shutdown( void )
{
   sk_hash_free( sgTtsQueueHash );
}
/*
 ******************************************************************************
 */
int tts_queue_add( void* context, const char* key )
{
   int i;
   for ( i = 0; i < TTS_QUEUE_SIZE; ++i )
   {
      if ( sgTtsQueue[i].busy == 0 )
         break;
   }
   if ( i == TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The TTS requests' queue is full!" );
//      tts_queue_debug();
      return -1;
   }
   else
   {
      sk_log( SK_DEBUG, "Adding the next element to the TTS queue at %d. Current head: %d",
                                                       i, sgTtsQueueHead );
      // Update hash
      sk_hash_add( sgTtsQueueHash, sk_hash_string( key ), i );

      sgTtsQueue[sgTtsQueueTail].next = i;
      sgTtsQueue[i].next = -1;
      sgTtsQueueTail = i;
      sgTtsQueue[i].busy = 1;
      sgTtsQueue[i].context = context;
      sgTtsQueue[i].key = key;
      sgTtsQueue[i].status = TTS_QUEUE_STATUS_IDLE;
      sgTtsQueue[i].request_time = time( NULL );
      if ( sgTtsQueueHead == -1 )
         sgTtsQueueHead = i;
   }
   return i;
}

/*
 ******************************************************************************
 */
int tts_queue_get_index( const char* key )
{
   int hash = sk_hash_string ( key );
   int i = sk_hash_get_first ( sgTtsQueueHash, hash );

   while ( i >= 0 )
   {
      if ( !strcmp( key, sgTtsQueue[i].key ) )
      {
         return i;
      }

      i = sk_hash_get_next ( sgTtsQueueHash, i );
   }

   return i;
}
/*
 ******************************************************************************
 */
void tts_queue_remove_head( void )
{
   if ( sgTtsQueueHead >= 0 && sgTtsQueue[sgTtsQueueHead].busy )
   {
      // Update hash
      int hash = sk_hash_string( sgTtsQueue[sgTtsQueueHead].key );
      sk_hash_remove( sgTtsQueueHash, hash, sgTtsQueueHead );

      sgTtsQueue[sgTtsQueueHead] = sgcTtsQueueEntryInitializer;
      sgTtsQueueHead = sgTtsQueue[sgTtsQueueHead].next;
   }
}
/*
 ******************************************************************************
 */
void tts_queue_remove( int index )
{
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      int i;
      int hash = sk_hash_string( sgTtsQueue[index].key );
      // Update hash
      sk_hash_remove( sgTtsQueueHash, hash, index );

      for ( i = 0; i < TTS_QUEUE_SIZE; ++i )
      {
         if ( sgTtsQueue[i].next == index )
         {
            sgTtsQueue[i].next = sgTtsQueue[index].next;
            break;
         }
      }
      if ( index == sgTtsQueueHead )
      {
         sgTtsQueueHead = sgTtsQueue[index].next;
      }
      if ( index == sgTtsQueueTail )
      {
         if ( i == TTS_QUEUE_SIZE )
            sgTtsQueueTail = 0;
         else
            sgTtsQueueTail = i;
      }

      sgTtsQueue[index] = sgcTtsQueueEntryInitializer;
   }
}

/*
 ******************************************************************************
 */
void tts_queue_set_status( int index, int status )
{
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      sgTtsQueue[index].status = status;
   }
}
/*
 ******************************************************************************
 */
int tts_queue_get_status( int index )
{
   int status = TTS_QUEUE_STATUS_UNDEFINED;
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      status = sgTtsQueue[index].status;
   }
   return status;
}

/*
 ******************************************************************************
 */
int tts_queue_is_empty( int index )
{
   int result = 0;
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      result = !sgTtsQueue[index].busy;
   }
   return result;
}
/*
 ******************************************************************************
 */
void tts_queue_set_context( int index, void* context )
{
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      sgTtsQueue[index].context = context;
   }
}

/*
 ******************************************************************************
 */
void* tts_queue_get_context( int index )
{
   void* ctx = NULL;
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      ctx = sgTtsQueue[index].context;
   }
   return ctx;
}

/*
 ******************************************************************************
 */
const char* tts_queue_get_key( int index )
{
   const char* key = NULL;
   if ( index < 0 || index >= TTS_QUEUE_SIZE )
   {
      sk_log( SK_WARNING, "The queue index is invalid: %d", index );
   }
   else
   {
      key = sgTtsQueue[index].key;
   }
   return key;
}
/*
 ******************************************************************************
 */

int tts_queue_get_indexes( int indexes[], int indexes_size, int status )
{
   int i = sgTtsQueueHead;
   int match_count = 0;
   for( ; ( i != -1 ) && ( match_count < indexes_size );
         i = sgTtsQueue[i].next )
   {
      if ( sgTtsQueue[i].status == status )
      {
         indexes[match_count++] = i;
      }
   }
   return match_count;
}

/*
 ******************************************************************************
 */
void tts_queue_debug()
{
   /*int i;
   sk_log_raw_data( "#################### TTS QUEUE DEBUG ########################\n" );
   sk_log_raw_data( " Index | Head | Tail | Busy | Status \n" );
   for ( i = 0; i < TTS_QUEUE_SIZE; ++i )
   {
      sk_log_raw_data_fmt( "   %d  |  %d  |  %d  |  %d  |   %d  \n",
            i, i == sgTtsQueueHead, i == sgTtsQueueTail, sgTtsQueue[i].busy, sgTtsQueue[i].status );
   }*/

}
