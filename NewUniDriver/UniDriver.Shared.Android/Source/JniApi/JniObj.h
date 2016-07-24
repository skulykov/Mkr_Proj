#ifndef MKR_MKRJNI_H_INCLUDED
#define MKR_MKRJNI_H_INCLUDED

#include <jni.h>

#define JNI_OBJ_NAME_LEN 		64
typedef struct
{
	jclass  obj_class;
	jobject obj;
	JavaVM* jvm;
	jint ver;
	char name[JNI_OBJ_NAME_LEN];
} android_jni_obj_type;

typedef struct
{
	JNIEnv*		env;
	jmethodID	mid;
} android_method_context_type;

void InitJNIObject(android_jni_obj_type* aObj, JNIEnv * aJNIEnv, jobject aJObj, const char* aObjName);

jmethodID InitJNIMethodContext(android_jni_obj_type* aObj, android_method_context_type* aMthdContext,
	const char* aMethodName, const char* aMethodSig);

void DisposeJNIObject(android_jni_obj_type* aObj);
void DisposeCustomJNIObject(android_jni_obj_type* aObj, jobject aCustomObj);
void CloseJNIObjects();

extern "C" {

	JNIEXPORT void JNICALL Java_com_mkr_unidriver_LoginActivity_initOnStart(JNIEnv * env, jobject obj);
	JNIEXPORT jint Java_com_mkr_unidriver_LoginActivity_loginReq(JNIEnv* env, jobject obj, jstring user, jstring pass, jint rid);


	JNIEXPORT jstring Java_com_mkr_unidriver_LoginActivity_getJniString(JNIEnv* env, jobject obj);

};

#endif

