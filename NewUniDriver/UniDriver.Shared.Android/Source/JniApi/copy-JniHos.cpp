#include "jnihos.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include <iostream>

#include "../mqtt_client/ThreadMqtt.h"
#include "../BizLogic/hos_interface.h"
#include "Global.h"


//===========================HosActivity======================================================================================
JNIEXPORT jint Java_com_mkr_service_MkrNLib_setDriverSts(JNIEnv* env, jobject obj,jint ind)
{
	int rv = 0;
	rv=process_driver_status((int) ind);
	return rv;
}


static My_Cache_Tlr *pC = 0;
//http://pickerwengs.blogspot.ca/2011/12/android-programming-objects-between.html
JNIEXPORT jobject Java_com_mkr_service_MkrNLib_getTLRList(JNIEnv* env, jobject obj, jstring date1, jstring date2, jint flag)
{
	std::vector<TimeLogRow*> oList;
	if(flag==0)
		oList = getTLRList();    // get all
	else if (flag == 1)
	{
		//jboolean isCopy;
		const char* date1_utf = env->GetStringUTFChars((jstring)date1, /*&isCopy*/NULL);
		oList = getAllTLRList4Date(string(date1_utf));
		/*The ReleaseString-Chars call is necessary whether GetStringChars has set *isCopy to JNI_TRUE or JNI_FALSE. 
		ReleaseStringChars either frees the copy or unpins the instance, depending upon whether GetStringChars has returned a copy or not.
		*/
		env->ReleaseStringUTFChars(date1, date1_utf);
	}

	if (pC == 0)
	{
		pC = new My_Cache_Tlr(env);
		/// Part1: java.util.ArrayList
		/*Note that jfieldIDs and jmethodIDs are opaque types, not object references, and should not be passed to NewGlobalRef
		http://developer.android.com/training/articles/perf-jni.html
		*/
		jclass clsArrayList = env->FindClass("java/util/ArrayList");   // local reference
		pC->clsArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(clsArrayList));  // global reference !!!
		pC->constructor = env->GetMethodID(pC->clsArrayList, "<init>", "()V");

		pC->arrayListAdd = env->GetMethodID(pC->clsArrayList, "add", "(Ljava/lang/Object;)Z");

		/// Part2: android.graphics.Rect
		jclass clsTLR= env->FindClass("com/mkr/models/TimeLogRow");
		pC->clsTLR = reinterpret_cast<jclass>(env->NewGlobalRef(clsTLR));

		pC->constructorTLR = env->GetMethodID(pC->clsTLR, "<init>", "()V");

		pC->field_Event = env->GetFieldID(pC->clsTLR, "Event", "I");
		pC->dt_Event = env->GetFieldID(pC->clsTLR, "dt", "J"); // long - J //http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html

		env->DeleteLocalRef(clsArrayList);
		env->DeleteLocalRef(clsTLR);
	}
	jobject objArrayList;
	//try {   -> Does not catch exceptions ???
		objArrayList = env->NewObject(pC->clsArrayList, pC->constructor, "");
	//}
	/*catch (...)   
	{
		string what = "Unidentified ex..";// string(ex.what());
		if (what.empty())
		{

		}
	}*/
	

	if (pC->field_Event == NULL||pC->field_Event == NULL) {
		//LOGE("Can't find %s.%s", f->class_name, f->field_name);
		return objArrayList;
	}

	for (int i = 0; i < oList.size(); ++i) {
		/// Part3: Create the new rect and add it into ArrayList
		jobject obj = env->NewObject(pC->clsTLR, pC->constructorTLR, "");

		int ev = 0;
		int64_t dt = 0;
		TimeLogRow *tlr = oList[i];
		if (tlr != 0)
		{
			ev =(int) tlr->Event;
			dt = (int64_t)tlr->LogTime.getMillis();
		}
	
		env->SetIntField(obj, pC->field_Event, ev);
		env->SetLongField(obj, pC->dt_Event, dt);

		env->CallBooleanMethod(objArrayList, pC->arrayListAdd, obj);
	}

	CleanUP_TLRList(oList);

	return objArrayList;

}

//------------------------Recap--------------------------
static My_Cache_Recap *pR = 0;

//http://pickerwengs.blogspot.ca/2011/12/android-programming-objects-between.html
JNIEXPORT jobject Java_com_mkr_service_MkrNLib_getRecapList(JNIEnv* env, jobject obj)
{
	std::vector<RecapRow*> oList;
	oList = getRecapList();    // get all
	if (pR == 0)
	{
		pR = new My_Cache_Recap(env);
		/// Part1: java.util.ArrayList
		/*Note that jfieldIDs and jmethodIDs are opaque types, not object references, and should not be passed to NewGlobalRef
		http://developer.android.com/training/articles/perf-jni.html
		*/
		jclass clsArrayList = env->FindClass("java/util/ArrayList");   // local reference
		pR->clsArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(clsArrayList));  // global reference !!!
		pR->constructor = env->GetMethodID(pR->clsArrayList, "<init>", "()V");

		pR->arrayListAdd = env->GetMethodID(pR->clsArrayList, "add", "(Ljava/lang/Object;)Z");

		/// Part2: android.graphics.Rect
		jclass clsRecap = env->FindClass("com/mkr/models/RecapRow");
		pR->clsRecap = reinterpret_cast<jclass>(env->NewGlobalRef(clsRecap));

		pR->constructorRecap = env->GetMethodID(pR->clsRecap, "<init>", "()V");

		pR->date = env->GetFieldID(pR->clsRecap, "date", "J"); // long - J //http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html
		pR->today = env->GetFieldID(pR->clsRecap, "today", "I");
		pR->cycleTotal = env->GetFieldID(pR->clsRecap, "cycleTotal", "I");
		pR->cycleAvailable = env->GetFieldID(pR->clsRecap, "cycleAvailable", "I");
		pR->cycle= env->GetFieldID(pR->clsRecap, "cycle", "Ljava/lang/String;"); 

		env->DeleteLocalRef(clsArrayList);
		env->DeleteLocalRef(clsRecap);
	}
	jobject objArrayList;
	//try {   -> Does not catch exceptions ???
	objArrayList = env->NewObject(pR->clsArrayList, pR->constructor, "");
	//}
	/*catch (...)
	{
	string what = "Unidentified ex..";// string(ex.what());
	if (what.empty())
	{

	}
	}*/
	if (pR->date == NULL || pR->cycle == NULL) {
		//LOGE("Can't find %s.%s", f->class_name, f->field_name);
		return objArrayList;
	}

	for (int i = 0; i < oList.size(); ++i) {
		/// Part3: Create the new rect and add it into ArrayList
		jobject obj = env->NewObject(pR->clsRecap, pR->constructorRecap, "");

		RecapRow *r = oList[i];
		if (r != 0)
		{
			env->SetLongField(obj, pR->date, r->date.getMillis());
			env->SetIntField(obj, pR->today,r->today);
			env->SetIntField(obj, pR->cycleTotal, r->cycleTotal);
			env->SetIntField(obj, pR->cycleAvailable, r->cycleAvailable);

			jstring _str = env->NewStringUTF(r->cycle.c_str());
			env->SetObjectField(obj, pR->cycle, _str);
		}
		env->CallBooleanMethod(objArrayList, pR->arrayListAdd, obj);
	}
	return objArrayList;
}
