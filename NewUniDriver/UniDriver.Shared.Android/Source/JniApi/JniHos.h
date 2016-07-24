#ifndef MKR_JNIHOS_H_INCLUDED
#define MKR_JNIHOS_H_INCLUDED

#include <jni.h>

extern "C" {

	
	JNIEXPORT jint Java_com_mkr_service_MkrNLib_setDriverSts(JNIEnv* env, jobject obj, jbyteArray barray,jint edit_flag);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getLastDriverStatus(JNIEnv* env, jobject obj);

	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getTLRList(JNIEnv* env, jobject obj,jlong date1,jlong date2,jint flag);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getRecapList(JNIEnv* env, jobject obj);

	JNIEXPORT jint Java_com_mkr_service_MkrNLib_saveEmployeeRow(JNIEnv* env, jobject obj, jbyteArray barray);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getEmployee(JNIEnv* env, jobject obj);
	
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getStringFromCurrentHosCycle(JNIEnv* env, jobject obj);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getHosCycleList(JNIEnv* env, jobject obj);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getHosRecapSummary(JNIEnv* env, jobject obj);

	JNIEXPORT jint Java_com_mkr_service_MkrNLib_updateHosCycle(JNIEnv* env, jobject obj,jint cycle);
	JNIEXPORT jint Java_com_mkr_service_MkrNLib_checkIfLoginIsNeeded(JNIEnv* env, jobject obj);

	JNIEXPORT jint Java_com_mkr_service_MkrNLib_sendEmail(JNIEnv* env, jobject obj, jbyteArray barray, jint array_len);
};

#if 0
//==============================Cache for Reuse=============================
class  My_Cache_Tlr {
public:
	JavaVM *jvm;
	jint ver;
	jclass clsArrayList;
	jmethodID constructor;
	jmethodID arrayListAdd;
	jclass clsTLR;
	jmethodID constructorTLR;
	jfieldID field_Event;
	jfieldID dt_Event;

	My_Cache_Tlr(JNIEnv* env)
	{
		// Java VM
		env->GetJavaVM(&this->jvm);
		this->ver = env->GetVersion();
	}

	~My_Cache_Tlr()
	{
		JNIEnv* env;
		// Getting the environment
		int res = this->jvm->GetEnv((void**)&env, this->ver);

		if (res || !env)
		{
			//sk_log(SK_ERROR, "My_Cache_Tlr error_1 !");
			return;
		}

		env->DeleteGlobalRef(clsArrayList);
		env->DeleteGlobalRef(clsTLR);
	}
};

class  My_Cache_Recap {
public:
	JavaVM *jvm;
	jint ver;
	jclass clsArrayList;
	jmethodID constructor;
	jmethodID arrayListAdd;
	jclass clsRecap;
	jmethodID constructorRecap;
	
	jfieldID date;
	jfieldID today;
	jfieldID cycleTotal;
	jfieldID cycleAvailable;
	jfieldID cycle;

	My_Cache_Recap(JNIEnv* env)
	{
		// Java VM
		env->GetJavaVM(&this->jvm);
		this->ver = env->GetVersion();
	}

	~My_Cache_Recap()
	{
		JNIEnv* env;
		// Getting the environment
		int res = this->jvm->GetEnv((void**)&env, this->ver);

		if (res || !env)
		{
			//sk_log(SK_ERROR, "My_Cache_Tlr error_1 !");
			return;
		}

		env->DeleteGlobalRef(this->clsArrayList);
		env->DeleteGlobalRef(this->clsRecap);
	}
};
#endif // #if 0


#endif
