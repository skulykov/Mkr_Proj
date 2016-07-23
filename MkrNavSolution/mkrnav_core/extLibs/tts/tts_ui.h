/* tts.h - The interface for the text to speech UI related functionality
 */


#ifndef INCLUDE__TTS_UI__H
#define INCLUDE__TTS_UI__H
#ifdef __cplusplus
extern "C" {
#endif

void tts_ui_initialize( void );

const char** tts_ui_voices_labels( void );

const void** tts_ui_voices_values( void );

int tts_ui_count( void );

const void* tts_ui_voice_value( const char* voice_id );

const char* tts_ui_voice_label( const char* voice_id );

#ifdef __cplusplus
}
#endif
#endif // INCLUDE__TTS_UI__H
