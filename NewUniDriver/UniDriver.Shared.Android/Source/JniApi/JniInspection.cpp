#include "jniinspection.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include <iostream>

#include "../mqtt_client/ClsMqtt.h"
#include "../BizLogic/Inspection/insphelper.h"
#include "Global.h"
#include "../BizLogic/hos/hos_events.h"
#include "db_interface.h"

//--------------------Inspections---------------------------------------------------
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getInspections(JNIEnv* env, jobject obj,jint boxID,jint type, jint flag)
{//// type=0 - pretrip, 1- posttrip / flag 0-recent, 1 -old
	//LOGI("MkrNLib_getInspections type=%d flag:%d", type,flag);
	PInspectionRowList *inspL = InspHelper::Get()->ReadAllInspReportsFromDB(boxID,type, flag);
	//LOGI("MkrNLib_getInspections inspL=%p", inspL);   //Always use %p for pointers.
	if (inspL != 0&&inspL->list_size()>0)
	{
		jbyteArray arr = 0;
		//try {
			//LOGI("MkrNLib_getInspections serializing flag=%d", flag);
			string pld = inspL->SerializeAsString();
			int size = pld.length();
			//LOGI("MkrNLib_getInspections AFTER serializing flag=%d size=%d", flag,size);
			arr = env->NewByteArray(size);
			env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
			//LOGI("MkrNLib_getInspections done flag=%d", flag);
		//}
		/*catch (std::exception &ex)
		{
			string msg = ex.what();// +std::string(" Stk:") + ex.StackTrace;
			LOGE("MkrNLib_getInspections Exc:%s", msg.c_str());

		}
		catch (...)
		{
			string what = "MkrNLib_getInspections Unidentified ex..";// string(ex.what());
			if (what.empty())
			{

			}
			LOGE(what.c_str());
		}*/

		delete inspL;
		//LOGE("MkrNLib_getInspections return arr=%p",arr);
		return arr;
	}
	return 0;
}
//JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getCategories(JNIEnv* env, jobject obj, jint BoxID)
//{
//	
//	PInspectionCategoryRowList* cL = ReadAllCategories(BoxID, false);
//	if (cL != NULL)
//	{
//		string pld = cL->SerializeAsString();
//		int size = pld.length();
//
//		jbyteArray arr = env->NewByteArray(size);
//		env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
//
//		delete cL;
//		return arr;
//	}
//	return NULL;
//}
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getInspectionItemsByCategoryId(JNIEnv* env, jobject obj, jint catID)
{

	//PInspectionItemList* cL = ReadInspectionItemsByCategoryId(catID);                  // read from DB
	PInspectionItemList* cL = InspHelper::Get()->ReadInspectionItemsByCategoryIdFromCache(catID);  // read from cache 
	if (cL != NULL)
	{
		string pld = cL->SerializeAsString();
		int size = pld.length();

		jbyteArray arr = env->NewByteArray(size);
		env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());

		//delete cL;  -> if read from cache do NOT delete it here
		return arr;
	}
	return NULL;
}

JNIEXPORT jbyteArray Java_com_mkr_unidriver_inspection_InspHelper_getDefect(JNIEnv* env, jobject obj, jint iID)
{
	PInspectionItem* p = InspHelper::Get()->GetDefect(iID);  // read from cache 
	if (p != NULL)
	{
		string pld = p->SerializeAsString();
		int size = pld.length();
		jbyteArray arr = env->NewByteArray(size);
		env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
		return arr;
	}
	return NULL;
}

JNIEXPORT jbyteArray Java_com_mkr_unidriver_inspection_InspHelper_getCatRow(JNIEnv* env, jobject obj, jint positionInTheList)
{
	PInspectionCategoryRow * p = InspHelper::Get()->GetCatRow(positionInTheList);  // read from cache 
	if (p != NULL)
	{
		string pld = p->SerializeAsString();
		int size = pld.length();
		jbyteArray arr = env->NewByteArray(size);
		env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
		return arr;
	}
	return NULL;
}
//-----------------------------------------------------------
JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_addDefect(JNIEnv* env, jobject obj, jbyteArray barray)
{
	jbyte* bufferPtr = env->GetByteArrayElements(barray, NULL);
	jsize lengthOfArray = env->GetArrayLength(barray);

	//Having jbyte* and length, you can do all the things in c-array.
	PInspectionItem ii;
	ii.ParseFromArray(bufferPtr, lengthOfArray);
	InspHelper::Get()->addDefect(&ii);
	//Finally, releasing it: 
	env->ReleaseByteArrayElements(barray, bufferPtr, 0);
	return 1;
}

JNIEXPORT jboolean Java_com_mkr_unidriver_inspection_InspHelper_isContainsDefects(JNIEnv* env, jobject obj)
{
	return InspHelper::Get()->isContainsDefects();
}
JNIEXPORT jboolean Java_com_mkr_unidriver_inspection_InspHelper_processItemDefectsInSubCats(JNIEnv* env, jobject obj,jint iID)
{
	return InspHelper::Get()->isContainsDefects();
}
JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_setAllChecked(JNIEnv* env, jobject obj, jint flag)
{
	return InspHelper::Get()->setAllChecked(flag);
}
JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_getAllChecked(JNIEnv* env, jobject obj,jint odo)
{
	return InspHelper::Get()->getAllCheckedAndSaveInspection(odo);
}

JNIEXPORT jint Java_com_mkr_unidriver_inspection_InspHelper_getIIColor(JNIEnv* env, jobject obj, jint iID)
{
	return InspHelper::Get()->getIIColor(iID);
}
JNIEXPORT /*jbyteArray*/jint Java_com_mkr_unidriver_inspection_InspHelper_createInspection(JNIEnv* env, jobject obj, jint InsRID,jint is_new,jint BoxID,jint inspType)
{
	PInspectionCategoryRowList* cL = InspHelper::Get()->CreateInspection(InsRID,is_new,BoxID, inspType);
	
	if (cL != NULL)
	{
		//string pld = cL->SerializeAsString();
		//int size = pld.length();

		//jbyteArray arr = env->NewByteArray(size);
		//env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());

		//delete cL; -> we keep it as a reference in InspHelper class
		return cL->list_size();  //arr;
	}
	return 0;
}
