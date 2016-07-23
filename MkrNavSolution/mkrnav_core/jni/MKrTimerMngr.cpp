/* MkRlNativeTimerManager_JNI.c - JNI STUB for the MkRlNativeTimerManager class

 */
#include "MkRNativeTimerManager_JNI.h"
#include "MkRJNI.h"
#include "mkrnav.h"
// The JNI object representing the current class
static android_jni_obj_type gJniObj;


#define JNI_CALL_MkRlNativeTimerManager_AddTask 		"AddTask"
#define JNI_CALL_MkRlNativeTimerManager_AddTask_Sig 	"(III)V"

#define JNI_CALL_MkRlNativeTimerManager_RemoveTask 		"RemoveTask"
#define JNI_CALL_MkRlNativeTimerManager_RemoveTask_Sig 	"(I)V"

#define JNI_CALL_MkRlNativeTimerManager_ShutDown 		"ShutDown"
#define JNI_CALL_MkRlNativeTimerManager_ShutDown_Sig 	"()V"


/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeTimerManager_InitTimerManagerNTV()
 * Initializes the JNI environment object for the MkRlNativeTimerManager class
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeTimerManager_InitTimerManagerNTV
  ( JNIEnv * aJNIEnv, jobject aJObj )
{
    JNI_LOG( SK_INFO, "Initializing JNI object MkRlNativeTimerManager" );
	// JNI object initialization
	InitJNIObject( &gJniObj, aJNIEnv, aJObj, "MkRlNativeTimerManager" );
}

/*************************************************************************************************
 * MkRlNativeTimerManager_AddTask()
 * Schedules the new timer
 * aIndex - the timer index
 * aTimerMsg - the timer message to be posted to the main thread upon expiration
 * aInterval - the expiration interval
 *
 */
void MkRlNativeTimerManager_AddTask( int aIndex, int aTimerMsg, int aInterval )
{
	JNIEnv *env;
	jmethodID mid;
	android_method_context_type lMthdContext;
	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeTimerManager_AddTask );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeTimerManager_AddTask,
																JNI_CALL_MkRlNativeTimerManager_AddTask_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}
	// Call the method
	lMthdContext.env->CallVoidMethod(  gJniObj.obj, lMthdContext.mid, aIndex, aTimerMsg, aInterval );
}
/*************************************************************************************************
 * MkRlNativeTimerManager_RemoveTask()
 * Removing the timer task according to the id
 *
 */
void MkRlNativeTimerManager_RemoveTask( int aIndex )
{

	JNIEnv *env;
	jmethodID mid;
	android_method_context_type lMthdContext;

	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeTimerManager_RemoveTask );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeTimerManager_RemoveTask,
																	JNI_CALL_MkRlNativeTimerManager_RemoveTask_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	// Call the method
	lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid, aIndex );

}

/*************************************************************************************************
 * MkRlNativeTimerManager_ShutDown()
 * Stops all the tasks and closes the manager
 *
 */
void MkRlNativeTimerManager_ShutDown()
{

	JNIEnv *env;
	jmethodID mid;
	android_method_context_type lMthdContext;

	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeTimerManager_ShutDown );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeTimerManager_ShutDown,
																	JNI_CALL_MkRlNativeTimerManager_ShutDown_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	// Call the method
	lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid );

}

/*************************************************************************************************
 * MkRlNativeTimerManager_DisposeRefs()
 * Dispose the JNI object of the MkRlNativeTimerManager module
 *
 */
void MkRlNativeTimerManager_DisposeRefs()
{
    JNI_LOG( SK_INFO, "Disposing the references for the JNI object %s", gJniObj.name );
	DisposeJNIObject( &gJniObj );
}
