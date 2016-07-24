#ifndef MKR_MKRNATIVEMANAGER_H_INCLUDED
#define MKR_MKRNATIVEMANAGER_H_INCLUDED

#include <jni.h>
#include <android\asset_manager_jni.h>
#include "JniObj.h"



extern "C" {
	JNIEXPORT void Java_com_mkr_service_MkrNVMng_createAssetManager(JNIEnv* env, jobject aJObj, jobject assetManager);

	JNIEXPORT void JNICALL Java_com_mkr_service_MkrNVMng_InitNativeMng
		(JNIEnv* aJNIEnv, jobject aJObj, jstring aCfgPath, jint aBuildSdkVersion,
			jstring aDeviceName, jstring aDeviceModel, jstring aDeviceManufacturer);
	JNIEXPORT void JNICALL Java_com_mkr_service_MkrNVMng_NativeMsgDispatcherNTV(JNIEnv* aJNIEnv, jobject aJObj, jint aMsgId);

};
#endif
