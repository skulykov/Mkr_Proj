/* MkrJNI.c - JNI management module
 *
 */

#include "MkrJNI.h"
#include <stdio.h>
#include "../core/mkrnav.h"
#include <android\asset_manager_jni.h>

//================================================================================================
//http://stackoverflow.com/questions/11103692/android-ndk-using-assetmanager-in-native-code
//http://stackoverflow.com/questions/13184772/crash-when-calling-aassetmanager-fromjava-jni-warning-instance-fieldid-0x5718

AAssetManager* assetMgr = NULL; 



AAssetManager* getAssetMng(){return assetMgr;}

/*************************************************************************************************
 * InitJNI
 * Initializes the android JNI object
 *
 */
void InitJNIObject( android_jni_obj_type* aObj, JNIEnv * aJNIEnv, jobject aJObj, const char* aObjName )
{
	// TODO :: Add log here
	jclass local_cls;
	aObj->obj = aJNIEnv->NewGlobalRef( aJObj );
	local_cls = aJNIEnv->GetObjectClass(aJObj );
	// Current class
	aObj->obj_class = (jclass)aJNIEnv->NewGlobalRef( local_cls );
	// JNI version
	aObj->ver = aJNIEnv->GetVersion();
	// Java VM
	aJNIEnv->GetJavaVM( &aObj->jvm );
	// Object name
	strncpy((char*) aObj->name, aObjName, JNI_OBJ_NAME_LEN-1 );
}

/*************************************************************************************************
 * InitJNIMethodContext
 * Initializes the method context
 * Returns the method id if successful. 0 if failure
 */
jmethodID InitJNIMethodContext( android_jni_obj_type* aObj, android_method_context_type* aMthdContext,
													const char* aMethodName, const char* aMethodSig )
{
	JNIEnv *env;
	int res = 0;
	jmethodID mid = 0;

	if ( !aObj->jvm )
	{
		sk_log( SK_ERROR, "Cannot find VM handle for JNI object %s!", aObj->name );
		// TODO :: Add log here
		return mid;
	}
	// Getting the environment
	res = aObj->jvm->GetEnv(  (void**)&env, aObj->ver );

	if ( res || !env )
	{
		sk_log( SK_ERROR, "Cannot obtain the Java environment for JNI object %s! %d %x", aObj->name, res, env );
		return mid;
	}

	mid = env->GetMethodID(  aObj->obj_class, aMethodName, aMethodSig );

	if ( !mid )
	{
		sk_log( SK_ERROR, "Cannot obtain the java function pointer for JNI object %s!", aObj->name );
		return mid;
	}
	// Init the context
	aMthdContext->env = env;
	aMthdContext->mid = mid;
	return mid;
}

/*************************************************************************************************
 * DisposeJNIObject
 * Deallocates the jni object
 *
 */
void DisposeJNIObject( android_jni_obj_type* aObj )
{
	JNIEnv *env;
	int res = 0;
	jmethodID mid = 0;

	if ( !aObj->jvm )
	{
		sk_log( SK_ERROR, "Cannot find VM handle for JNI object %s!", aObj->name );
		// TODO :: Add log here
		return;
	}
	// Getting the environment
	res = aObj->jvm->GetEnv(  (void**)&env, aObj->ver );

	if ( res || !env )
	{
		sk_log( SK_ERROR, "Cannot obtain the Java environment for JNI object %s!", aObj->name );
		return;
	}
	// Dispose the references
	env->DeleteGlobalRef(  aObj->obj );
	env->DeleteGlobalRef( aObj->obj_class );
}


/*************************************************************************************************
 * DisposeCustomJNIObject
 * Deallocates the custom jni object using the environment data passed in the android JNI object
 *  aObj 		- android JNI object
 *  aCustomObj 	- custom object to be deallocated
 */
void DisposeCustomJNIObject( android_jni_obj_type* aObj, jobject aCustomObj )
{
	JNIEnv *env;
	int res = 0;
	jmethodID mid = 0;

	if ( !aObj->jvm )
	{
		sk_log( SK_ERROR, "Cannot find VM handle for JNI object %s!", aObj->name );
		// TODO :: Add log here
		return;
	}
	// Getting the environment
	res = aObj->jvm->GetEnv( (void**)&env, aObj->ver );

	if ( res || !env )
	{
		sk_log( SK_ERROR, "Cannot obtain the Java environment for JNI object %s!", aObj->name );
		return;
	}
	// Dispose the references
	env->DeleteGlobalRef( aCustomObj );
}


/*************************************************************************************************
 * CloseJNIObjects
 * Disposes all the JNI objects in the system
 *
 */
void CloseJNIObjects()
{
	/*FreeMapNativeManager_DisposeRefs();
	FreeMapNativeCanvas_DisposeRefs();
	FreeMapNativeSoundManager_DisposeRefs();
	FreeMapNativeTimerManager_DisposeRefs();
	MkrMenuManager_DisposeRefs();*/
	MkrMsgBox_DisposeRefs();
	/*MkrSpeechttManager_DisposeRefs();
	MkrSoundRecorder_DisposeRefs();
	MkrResManager_DisposeRefs();
	MkrTtsManager_DisposeRefs();
#ifdef ANDROID_WIDGET
	MkrAppWidgetManager_DisposeRefs();
#endif*/
}

