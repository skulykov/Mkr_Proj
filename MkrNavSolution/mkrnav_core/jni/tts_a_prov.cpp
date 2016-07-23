/* tts_android_provider.c - android native tts implementation

 */

#include <string.h>
#include "mkrnav.h"
#include "../tts/tts.h"
#include "../tts/tts_provider.h"
#include "tts_android_provider.h"
#include "MkrJNI.h"

//======================== Local defines ========================

#define ANDROID_TTS_PROVIDER                    "a_tts"
#define ANDROID_TTS_PROVIDER_VOICES             "voices_a_tts.csv"

//======================== Local types ========================

typedef struct
{
   const void* cb_context;
   TtsSynthResponseCb response_cb;
   const char* text;
   BOOL        busy;
   int         index;
} AndroidTtsContext;
//======================== Globals ========================
static AndroidTtsContext    sgCtxPool[TTS_ACTIVE_REQUESTS_LIMIT];
static char                 sgVoicesPath[TTS_PATH_MAXLEN];
//======================== Local Declarations ========================

static void _synth_request( const void* context, TtsTextList text_list, const TtsSynthRequestParams* params, TtsSynthResponseCb response_cb );
static AndroidTtsContext* _allocate_context( void );

/*
 ******************************************************************************
 */
void tts_android_provider_init( void )
{
   int i;
   TtsProvider provider;

   sk_path_format( sgVoicesPath, TTS_PATH_MAXLEN, sk_path_tts(), ANDROID_TTS_PROVIDER_VOICES );
   provider.batch_request_limit = 1;
   provider.provider_name = ANDROID_TTS_PROVIDER;
   provider.storage_type = __tts_db_data_storage__file;
   provider.request_cb = _synth_request;
   provider.voices_cfg = sgVoicesPath;
   provider.prepare_cb = MkRlTtsManager_Prepare;
   provider.concurrent_limit = -1;

   // Register the android provider
   tts_register_provider( &provider );

   // Initialize the context pool
   for ( i = 0; i < TTS_ACTIVE_REQUESTS_LIMIT; ++i )
   {
      sgCtxPool[i].busy = FALSE;
      sgCtxPool[i].index = i;
   }

   //svk add
   MkRlTtsManager_Prepare();
}

/*
 ******************************************************************************
 * Synth request callback. See TtsSynthRequestCb prototype
 */
static void _synth_request( const void* context, TtsTextList text_list, const TtsSynthRequestParams* params, TtsSynthResponseCb response_cb )
{
   AndroidTtsContext* ctx;
   int i, index;
   const TtsPath* tts_path;

   for ( i = 0; i < TTS_BATCH_REQUESTS_LIMIT; ++i )
   {
      if ( text_list[i] )
      {
         if ( ( ctx = _allocate_context() ) )
         {
            tts_path = params->path_list[i];
            ctx->cb_context = context;
            ctx->response_cb = response_cb;
            ctx->text = text_list[i];
            MkRlTtsManager_Commit( text_list[i], tts_path->path, ctx );
         }
      }
   }
}

static void _synth_request( const char *text, const char *path, TtsSynthResponseCb response_cb )
{
   AndroidTtsContext* ctx;
   int i, index;
  
      if ( text )
      {
         if ( ( ctx = _allocate_context() ) )
         {
            
            ctx->cb_context = 0;
            ctx->response_cb = response_cb;
            ctx->text = text;
            MkRlTtsManager_Commit( text, path, ctx );
         }
      }
  
}

/*
 ******************************************************************************
 * Allocates context from the contexts pool
 */
static AndroidTtsContext* _allocate_context( void )
{
   int i;
   AndroidTtsContext* ctx = NULL;

   for ( i = 0; i < TTS_ACTIVE_REQUESTS_LIMIT; ++i )
   {
      if ( !sgCtxPool[i].busy )
      {
         ctx = &sgCtxPool[i];
         ctx->busy = TRUE;
         break;
      }
   }

   if ( i == TTS_ACTIVE_REQUESTS_LIMIT )
   {
      sk_log( SK_ERROR, "There is on available context in Android TTS request context pool!" );
   }
   return ctx;
}
/*
 ******************************************************************************
 */
void tts_android_provider_response( const void* context, int res_status )
{
   // Break constancy to update the context
   AndroidTtsContext* android_ctx = (AndroidTtsContext*) context;
   TtsSynthResponseData response_data;
   response_data.count = 1;
   response_data.text_list[0] = android_ctx->text;
   /*
    * The data stored externally
    */
   response_data.tts_data[0].data = NULL;

   if ( android_ctx->response_cb )
      android_ctx->response_cb( android_ctx->cb_context, res_status, &response_data );
   android_ctx->busy = FALSE;
}



const char *g_text="In 10 meters turn right on Lauderdale Drive";
const char *g_path="/mnt/sdcard/fcnavgl/tts_voice/test.wav";

void _cb_response( const void* context, int res_status, TtsSynthResponseData* response_data )
{
	sk_log( SK_ERROR, "_cb_response in Android TTS request GOT!" );
	MkRlSoundManager_PlayFile( g_path );
}
void test_tts()
{
	//_synth_request( g_text, g_path, _cb_response );
	MkRlTtsManager_Speak( g_text);
}