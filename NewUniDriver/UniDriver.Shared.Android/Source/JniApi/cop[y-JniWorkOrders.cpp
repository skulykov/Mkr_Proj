#include "JniWorkOrders.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include <iostream>

#include "../mqtt_client/ThreadMqtt.h"
#include "../BizLogic/workorders/workorders_interface.h"
#include "Global.h"


//===========================WorkOrdersActivity======================================================================================


static My_Cache_Woi *pC = 0;
//http://pickerwengs.blogspot.ca/2011/12/android-programming-objects-between.html
JNIEXPORT jobject Java_com_mkr_service_MkrNLib_getWOList(JNIEnv* env, jobject obj)
{
	std::vector<PWorkOrderItem*> oList;
	bool rv= getWoiList(oList);
		
	if (pC == 0)
	{
		pC = new My_Cache_Woi(env);
		/// Part1: java.util.ArrayList
		/*Note that jfieldIDs and jmethodIDs are opaque types, not object references, and should not be passed to NewGlobalRef
		http://developer.android.com/training/articles/perf-jni.html
		*/
		jclass clsArrayList = env->FindClass("java/util/ArrayList");   // local reference
		pC->clsArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(clsArrayList));  // global reference !!!
		pC->constructor = env->GetMethodID(pC->clsArrayList, "<init>", "()V");
		pC->arrayListAdd = env->GetMethodID(pC->clsArrayList, "add", "(Ljava/lang/Object;)Z");

		/// Part2: android.graphics.Rect
		jclass clsWoi= env->FindClass("com/mkr/models/WorkOrderItem");
		pC->clsWoi = reinterpret_cast<jclass>(env->NewGlobalRef(clsWoi));

		pC->constructorWoi = env->GetMethodID(pC->clsWoi, "<init>", "()V");

		pC->f_ID = env->GetFieldID(pC->clsWoi, "ID", "J");
		pC->f_Name = env->GetFieldID(pC->clsWoi, "Name", "Ljava/lang/String;");
		pC->f_CreatedTS = env->GetFieldID(pC->clsWoi, "CreatedTS", "J"); // long - J //http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html

		env->DeleteLocalRef(clsArrayList);
		env->DeleteLocalRef(clsWoi);
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
	

	if (pC->f_ID == NULL||pC->f_Name == NULL) {
		//LOGE("Can't find %s.%s", f->class_name, f->field_name);
		return objArrayList;
	}

	for (int i = 0; i < oList.size(); ++i) {
		/// Part3: Create the new obj and add it into ArrayList
		jobject obj = env->NewObject(pC->clsWoi, pC->constructorWoi, "");

		PWorkOrderItem *woi = oList[i];
		if (woi != 0)
		{
			env->SetLongField(obj, pC->f_ID, woi->id());
			env->SetLongField(obj, pC->f_CreatedTS, woi->createdts());
			jstring _str = env->NewStringUTF(woi->name().c_str());
			env->SetObjectField(obj, pC->f_Name, _str);
		}
		env->CallBooleanMethod(objArrayList, pC->arrayListAdd, obj);
	}

	CleanUP_WOList(oList);
	return objArrayList;

}


