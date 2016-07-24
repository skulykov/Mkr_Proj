#ifndef MKR_JNIHOS_H_INCLUDED
#define MKR_JNIHOS_H_INCLUDED

#include <jni.h>

extern "C" {
	
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getInspections(JNIEnv* env, jobject obj, jint boxID, jint type, jint flag);
	//JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getCategories(JNIEnv* env, jobject obj, jint BoxID);
	JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getInspectionItemsByCategoryId(JNIEnv* env, jobject obj, jint catID);

	JNIEXPORT /*jbyteArray*/jint Java_com_mkr_unidriver_inspection_InspHelper_createInspection(JNIEnv* env, jobject obj, jint irID, jint is_new,jint BoxID, jint inspType);
	JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_addDefect(JNIEnv* env, jobject obj, jbyteArray barray);
	JNIEXPORT jboolean Java_com_mkr_unidriver_inspection_InspHelper_isContainsDefects(JNIEnv* env, jobject obj);
	JNIEXPORT jboolean Java_com_mkr_unidriver_inspection_InspHelper_processItemDefectsInSubCats(JNIEnv* env, jobject obj, jint iID);
	
	JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_setAllChecked(JNIEnv* env, jobject obj, jint flag);
	JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_getAllChecked(JNIEnv* env, jobject obj, jint odo);

	JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_getIIColor(JNIEnv* env, jobject obj, jint iID);
	JNIEXPORT jbyteArray Java_com_mkr_unidriver_inspection_InspHelper_getDefect(JNIEnv* env, jobject obj, jint iID);

	JNIEXPORT jbyteArray Java_com_mkr_unidriver_inspection_InspHelper_getCatRow(JNIEnv* env, jobject obj, jint pos);
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
