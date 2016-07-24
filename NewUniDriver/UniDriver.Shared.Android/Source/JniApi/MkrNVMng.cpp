#include "MkrNVMng.h"
#include <stdio.h>
//#include "../core/mkrnav.h"

#include "Dbg.h"
#include <assert.h>


// The JNI object representing the current class
static android_jni_obj_type gJniObj;


AAssetManager* assetMgr = NULL;

JNIEXPORT void Java_com_mkr_service_MkrNVMng_createAssetManager(JNIEnv* env, jobject aJObj, jobject assetManager)
{
	AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
	assert(NULL != mgr);
	assetMgr = mgr;
	env->NewGlobalRef(assetManager); //won't be garbage collected.
}

AAssetManager* getAssetMng() { return assetMgr; }


JNIEXPORT void JNICALL Java_com_mkr_service_MkrNVMng_InitNativeMng
(JNIEnv* aJNIEnv, jobject aJObj, jstring aCfgPath, jint aBuildSdkVersion,
	jstring aDeviceName, jstring aDeviceModel, jstring aDeviceManufacturer)
{

	jboolean isCopy;
	const char* cfgPath = aJNIEnv->GetStringUTFChars(aCfgPath, &isCopy);
	const char* deviceName = aJNIEnv->GetStringUTFChars(aDeviceName, &isCopy);
	const char* deviceModel = aJNIEnv->GetStringUTFChars(aDeviceModel, &isCopy);
	const char* deviceManufacturer = aJNIEnv->GetStringUTFChars(aDeviceManufacturer, &isCopy);


	aJNIEnv->ReleaseStringUTFChars(aCfgPath, cfgPath);

	// The logger can be started only after the path initialization
	LOGI("Initializing JNI object MkRlNativeManager");
	InitJNIObject(&gJniObj, aJNIEnv, aJObj, "MkrNVMng");


	aJNIEnv->ReleaseStringUTFChars(aDeviceName, deviceName);
	aJNIEnv->ReleaseStringUTFChars(aDeviceModel, deviceModel);
	aJNIEnv->ReleaseStringUTFChars(aDeviceManufacturer, deviceManufacturer);
}
/*************************************************************************************************
* Java_net_mkrnav_MkRlNativeManager_NativeMsgDispatcherNTV
* JNI wrapper for the sk_main_message_dispatcher (will be called from the Java layer)
*
*/
JNIEXPORT void JNICALL Java_com_mkr_service_MkrNVMng_NativeMsgDispatcherNTV(JNIEnv* aJNIEnv, jobject aJObj, jint aMsgId)
{
	//sk_main_message_dispatcher(aMsgId);
}