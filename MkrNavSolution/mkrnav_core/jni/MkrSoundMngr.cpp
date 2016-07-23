/* MkRlSoundManager_JNI.c - Sound management
 
 */

#include "MKrSoundManager_JNI.h"
#include "MKrJNI.h"
#include "mkrnav.h"
#include <string.h>
// The JNI object representing the current class
static android_jni_obj_type gJniObj;


#define JNI_CALL_MkRlSoundManager_PlayFile                 "PlayFile"
#define JNI_CALL_MkRlSoundManager_PlayFile_Sig             "([B)V"

#define JNI_CALL_MkRlSoundManager_PlayBuffer                 "PlayBuffer"
#define JNI_CALL_MkRlSoundManager_PlayBuffer_Sig             "([B)V"

#define JNI_CALL_MkRlSoundManager_LoadSoundData                 "LoadSoundData"
#define JNI_CALL_MkRlSoundManager_LoadSoundData_Sig             "([B)V"


/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlSoundManager_InitSoundManagerNTV()
 * Initializes the JNI environment object for the MkRlSoundManager class
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlSoundManager_InitSoundManagerNTV
  ( JNIEnv * aJNIEnv, jobject aJObj )
{
	sk_log( SK_INFO, "Initializing JNI object MkRlSoundManager" );
	// JNI object initialization
	InitJNIObject( &gJniObj, aJNIEnv, aJObj, "MkRlSoundManager" );
}


/*************************************************************************************************
 * MkRlSoundManager_PlayFile()
 * Plays the sound file synchronously by calling the Java layer function JNI_CALL_MkRlSoundManager_PlayFile
 * aFileName - the full path to the sound file
 *
 */
void MkRlSoundManager_PlayFile( const char* aFileName )
{
	JNIEnv *env;
	jmethodID mid;
	jbyteArray byteArray;
	android_method_context_type lMthdContext;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlSoundManager_PlayFile );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlSoundManager_PlayFile,
														JNI_CALL_MkRlSoundManager_PlayFile_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	byteArray = lMthdContext.env->NewByteArray(  strlen( aFileName ) );
	lMthdContext.env->SetByteArrayRegion( byteArray, 0, strlen( aFileName ), (const jbyte*)aFileName );
	// Call the method
	lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid, byteArray );
}


/*************************************************************************************************
 * MkRlSoundManager_PlayBuffer()
 * Plays the sound buffer
 * aFileName - the full path to the sound file
 *
 */
void MkRlSoundManager_PlayBuffer( void* aBuf, int aSize )
{
   JNIEnv *env;
   jmethodID mid;
   jbyteArray byteArray;
   android_method_context_type lMthdContext;
   JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlSoundManager_PlayBuffer );
   mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlSoundManager_PlayBuffer,
                                                               JNI_CALL_MkRlSoundManager_PlayBuffer_Sig );
   if ( !mid || !lMthdContext.env )
   {
      sk_log( SK_ERROR, "Failed to obtain method context!" );
      return;
   }

   byteArray = lMthdContext.env->NewByteArray(  aSize );
   lMthdContext.env->SetByteArrayRegion(  byteArray, 0, aSize, (const jbyte*)aBuf );
   // Call the method
   lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid, byteArray );
}

/*************************************************************************************************
 * MkRlSoundManager_LoadSoundData()
 * PreLoads the sounds files to the memory
 * aDataDir - the full path to the sound resources directory
 *
 */
void MkRlSoundManager_LoadSoundData( char* aDataDir )
{
	JNIEnv *env;
	jmethodID mid;
	jbyteArray byteArray;
	android_method_context_type lMthdContext;
	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlSoundManager_LoadSoundData );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlSoundManager_LoadSoundData,
														JNI_CALL_MkRlSoundManager_LoadSoundData_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	byteArray = lMthdContext.env->NewByteArray(  strlen( aDataDir ) );
	lMthdContext.env->SetByteArrayRegion(  byteArray, 0, strlen( aDataDir ), (const jbyte*)aDataDir );
	// Call the method
	lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid, byteArray );
}


/*************************************************************************************************
 * MkRlSoundManager_DisposeRefs()
 * Dispose the JNI object of the MkRlSoundManager module
 *
 */
void MkRlSoundManager_DisposeRefs()
{
    JNI_LOG( SK_INFO, "Disposing the references for the JNI object %s", gJniObj.name );
	DisposeJNIObject( &gJniObj );
}
