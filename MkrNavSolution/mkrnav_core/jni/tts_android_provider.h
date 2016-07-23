/* tts_android_provider.h - The interface for the android native text to speech module
 *

 */


#ifndef INCLUDE__TTS_ANDROID_PROVIDER__H
#define INCLUDE__TTS_ANDROID_PROVIDER__H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initializes the android native tts provider. Registers the provider
 * on the tts engine
 * Params:  void
 *
 * Returns: void
 */
void tts_android_provider_init( void );
/*
 * Handles the response from the native provider
 *
 * Params:  context - internal context pointer
 *          res_status - the synthesis result status
 *
 * Returns: void
 */
void tts_android_provider_response( const void* context, int res_status );

#ifdef __cplusplus
}
#endif
#endif // INCLUDE__TTS_ANDROID_PROVIDER__H
