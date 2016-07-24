#ifndef MKR_JNIWOI_H_INCLUDED
#define MKR_JNIWOI_H_INCLUDED

#include <jni.h>


extern "C" {

	
	
	JNIEXPORT jobject Java_com_mkr_service_MkrNLib_getWOList(JNIEnv* env, jobject obj);
	
};


//==============================Cache for Reuse=============================
class  My_Cache_Woi {
public:
	JavaVM *jvm;
	jint ver;
	jclass clsArrayList;
	jmethodID constructor;
	jmethodID arrayListAdd;
	jclass clsWoi;
	jmethodID constructorWoi;
	jfieldID f_ID;
	jfieldID f_Name;
	jfieldID f_CreatedTS;

	My_Cache_Woi(JNIEnv* env)
	{
		// Java VM
		env->GetJavaVM(&this->jvm);
		this->ver = env->GetVersion();
	}

	~My_Cache_Woi()
	{
		JNIEnv* env;
		// Getting the environment
		int res = this->jvm->GetEnv((void**)&env, this->ver);

		if (res || !env)
		{
			//sk_log(SK_ERROR, "My_Cache_Woi error_1 !");
			return;
		}

		env->DeleteGlobalRef(clsArrayList);
		env->DeleteGlobalRef(clsWoi);
	}
};



#endif
