#ifndef MKR_JNIAPI_H_INCLUDED
#define MKR_JNIAPI_H_INCLUDED

#include <jni.h>

extern "C" {

	JNIEXPORT void JNICALL Java_com_mkr_service_MkrNLib_initOnStart(JNIEnv * env, jobject obj);
	JNIEXPORT jint Java_com_mkr_service_MkrNLib_loginReq(JNIEnv* env, jobject obj, jstring user, jstring pass,jint rid);
	
	JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_processLogout(JNIEnv * env, jobject obj);
	
	//JNIEXPORT jstring Java_com_mkr_service_MkrNLib_getJniString(JNIEnv* env, jobject obj);
	JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_sendRequest(JNIEnv * env, jobject obj, jint req);

	JNIEXPORT void JNICALL Java_com_mkr_service_MkrNLib_initMQTT(JNIEnv * env, jobject obj);

	JNIEXPORT jint Java_com_mkr_service_MkrNLib_sendMessage(JNIEnv* env, jobject obj, jstring topic, jstring msg, jint mid);
	JNIEXPORT jint Java_com_mkr_service_MkrNLib_write2Log(JNIEnv* env, jobject obj, jstring msg);
	//JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getProtoMessage(JNIEnv* env, jobject obj, jint msg_id);
	JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_registerReq(JNIEnv *env, jobject obj, jbyteArray barray, jint msg_id);

	JNIEXPORT jbyteArray JNICALL Java_com_mkr_service_MkrNLib_getVehicles(JNIEnv *env, jobject obj);
	JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_saveVehicle(JNIEnv *env, jobject obj, jbyteArray ba);

};

#endif
