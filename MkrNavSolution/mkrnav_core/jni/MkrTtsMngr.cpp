/* MkRlTtsManager_JNI.c - JNI STUB for the TtsManager class
 
 */
#include <string.h>
#include "MkrTtsManager_JNI.h"
#include "MkrJNI.h"
#include "../core/mkrnav.h"
#include "tts_android_provider.h"

// The JNI object representing the current class
static android_jni_obj_type gJniObj;


#define JNI_CALL_MkRlTtsManager_Submit          "Submit"
#define JNI_CALL_MkRlTtsManager_Submit_Sig      "(Ljava/lang/String;Ljava/lang/String;J)V"
#define JNI_CALL_MkRlTtsManager_Prepare         "Prepare"
#define JNI_CALL_MkRlTtsManager_Prepare_Sig     "()V"
#define JNI_CALL_MkRlTtsManager_Speak          "SpeakText"
#define JNI_CALL_MkRlTtsManager_Speak_Sig      "(Ljava/lang/String;)V"

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlTtsManagerBase_InitTtsManagerNTV()
 * Initializes the JNI environment object for the MkRlTtsManager class
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlTtsManager_InitTtsManagerNTV
( JNIEnv* aJNIEnv, jobject aJObj )
{
#if 0
	JNI_LOG( SK_INFO, "Initializing JNI object MkRlTtsManager" );
   // JNI object initialization
   InitJNIObject( &gJniObj, aJNIEnv, aJObj, "MkRlTtsManager" );

   tts_android_provider_init();
#endif
}
#if 0
/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlTtsManagerBase_TtsManagerCallbackNTV
 * Callback
 *
 */
JNIEXPORT void Java_com_mkr_navgl_service_MkRlTtsManager_TtsManagerCallbackNTV
  (JNIEnv * aJNIEnv, jobject aJObj, jlong aCbContext, jint res_status )
{
   tts_android_provider_response( (const void*) aCbContext, res_status );
}
/*************************************************************************************************
 * MkRlTtsManager_Commit()
 * Submits the synthesis request
 * aContext - callback context
 * aTimeout - timeout
 *
 */
void MkRlTtsManager_Commit( const char* aText, const char* aFullPath, void* aContext )
{
    android_method_context_type lMthdContext;
    jmethodID mid;
    jstring text, path;
    jlong ptrCtx;

    JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlTtsManager_Submit );
    mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlTtsManager_Submit,
                                                          JNI_CALL_MkRlTtsManager_Submit_Sig );
    if ( !mid || !lMthdContext.env )
    {
        sk_log( SK_ERROR, "Failed to obtain method context!" );
        return;
    }

    ptrCtx = (jlong) aContext;

    text = lMthdContext.env->NewStringUTF(  aText );
    path = lMthdContext.env->NewStringUTF(  aFullPath );

    // Calling the method
    lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid,
          text, path, ptrCtx );
}

void MkRlTtsManager_Speak( const char* aText)
{
    android_method_context_type lMthdContext;
    jmethodID mid;
    jstring text, path;
    jlong ptrCtx;

    JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlTtsManager_Speak );
    mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlTtsManager_Speak,
                                                          JNI_CALL_MkRlTtsManager_Speak_Sig );
    if ( !mid || !lMthdContext.env )
    {
        sk_log( SK_ERROR, "Failed to obtain method context!" );
        return;
    }

   
    text = lMthdContext.env->NewStringUTF(  aText );
   
    // Calling the method
    lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid, text);
}

/*************************************************************************************************
 * MkRlTtsManager_Prepare()
 * Prepares the Android TTS provider
 *
 */
void MkRlTtsManager_Prepare( void )
{
   android_method_context_type lMthdContext;
   jmethodID mid;

   JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlTtsManager_Prepare );
   mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlTtsManager_Prepare,
                                                            JNI_CALL_MkRlTtsManager_Prepare_Sig );
   if ( !mid || !lMthdContext.env )
   {
       sk_log( SK_ERROR, "Failed to obtain method context!" );
       return;
   }

   // Calling the method
   lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid );

}
/*************************************************************************************************
 * MkRlTtsManager_DisposeRefs()
 * Dispose the JNI object of the MkRlTtsManager module
 *
 */
void MkRlTtsManager_DisposeRefs( void )
{
   JNI_LOG( SK_INFO, "Disposing the references for the JNI object %s", gJniObj.name );
   DisposeJNIObject( &gJniObj );
}
#endif