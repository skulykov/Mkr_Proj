#include "jnihos.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include "../Network/NetComm.h"
#include <iostream>

#include "../mqtt_client/ClsMqtt.h"
#include "../BizLogic/hos/hos_interface.h"
#include "Global.h"
#include "../BizLogic/hos/hos_events.h"
//#include "db_interface.h"


string GetHosRecapSummary();

//===========================HosActivity======================================================================================
static jbyteArray ba_from_string(JNIEnv* env,string str)
{
	int size = str.length();
	jbyteArray arr = env->NewByteArray(size);
	env->SetByteArrayRegion(arr, 0, size, (jbyte*)str.c_str());
	return arr;
}
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getHosRecapSummary(JNIEnv* env, jobject obj)
{
	return ba_from_string(env, GetHosRecapSummary());
}
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getHosCycleList(JNIEnv* env, jobject obj)
{
	return ba_from_string(env,GetHosCycleList());
}
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getStringFromCurrentHosCycle(JNIEnv* env, jobject obj)
{
	return ba_from_string(env,GetStringFromCurrentHosCycle());
}
JNIEXPORT jint Java_com_mkr_service_MkrNLib_setDriverSts(JNIEnv* env, jobject obj, jbyteArray barray,jint edit_flag)
{
	int rv = 0;
	jbyte* bufferPtr = env->GetByteArrayElements(barray, NULL);
	jsize lengthOfArray = env->GetArrayLength(barray);
	
	PTimeLogRow ptlr;
	ptlr.ParseFromArray(bufferPtr, lengthOfArray);
	
	if (edit_flag == 1)
		rv=process_driver_status(&ptlr,false);
	else
		rv = process_driver_status(&ptlr);
	
	env->ReleaseByteArrayElements(barray, bufferPtr, 0);
	if (edit_flag == 1)
	{
		
		RestructureHosEvents();
		
	}
	return rv;
}
JNIEXPORT jint Java_com_mkr_service_MkrNLib_sendEmail(JNIEnv* env, jobject obj, jbyteArray barray,jint array_len)
{
	int rv = 0;
	jbyte* bufferPtr = env->GetByteArrayElements(barray, NULL);
	jsize lengthOfArray = env->GetArrayLength(barray);
	if (lengthOfArray == array_len)
	{
		rv=send_email(bufferPtr, lengthOfArray);
	}
	else {
		rv = -1;
	}
	
	env->ReleaseByteArrayElements(barray, bufferPtr, 0);
	return rv;
}

JNIEXPORT jint Java_com_mkr_service_MkrNLib_saveEmployeeRow(JNIEnv* env, jobject obj, jbyteArray barray)
{
	jbyte* bufferPtr = env->GetByteArrayElements(barray, NULL);
	jsize lengthOfArray = env->GetArrayLength(barray);

	//Having jbyte* and length, you can do all the things in c-array.
	EmployeeRow *emp=new EmployeeRow();                  // DO NOT DELETE emp object as we keep it as a reference to Global::prof
	emp->ParseFromArray(bufferPtr, lengthOfArray);
	Global::Get()->UpdateCurrentProfile(emp);//EmployeeDB::Get()->SaveEmployee(emp);
	//Finally, releasing it: 
	env->ReleaseByteArrayElements(barray, bufferPtr, 0);
	return 1;
}

JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getEmployee(JNIEnv* env, jobject obj)
{
	EmployeeRow *emp= Global::Get()->GetCurrentProfile();
	if (emp != NULL)
	{
		string pld = emp->SerializeAsString();
		return ba_from_string(env,pld);
	}
	return NULL;
}

JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getLastDriverStatus(JNIEnv* env, jobject obj)
{
	PTimeLogRow *tlr = getLastDriverStatus();
	if (tlr != NULL)
	{
		string pld = tlr->SerializeAsString();
		delete tlr;
		return ba_from_string(env, pld);
	}
	return NULL;
}


//http://pickerwengs.blogspot.ca/2011/12/android-programming-objects-between.html
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getTLRList(JNIEnv* env, jobject obj, jlong date1, jlong date2, jint flag)
{
	PTimeLogRowList* oList=0;
	if (flag == 0)
	{
		oList = getTLRList();    // get all
	}
	else if (flag == 1)
	{
		oList = getAllTLRList4Date(date1);
		if (oList&&oList->list_size() == 0)
		{
			// create at midnight
			TimeLogRow *tlr = HosEvents_CreateAtMidNight(date1);
			oList->mutable_list()->AddAllocated(tlr);
		}
	}
	else if (flag == 2)
	{
		oList =  new PTimeLogRowList();
		TimeLogRow *tlr = HosEvents_CreateAtMidNight(date1,false,12*60*60000);   // create at noon
		oList->mutable_list()->AddAllocated(tlr);
	}
	
	if (oList&&oList->list_size() > 0)
	{	
		string pld= oList->SerializeAsString();
		jbyteArray arr = ba_from_string(env,pld);
		
		delete oList;
		return arr;
	}
	if (oList != 0)
		delete oList;
	return NULL;
}
JNIEXPORT jint Java_com_mkr_service_MkrNLib_updateHosCycle(JNIEnv* env, jobject obj, jint cycle)
{
	return UpdateHosCycle(cycle);
}
JNIEXPORT jint Java_com_mkr_service_MkrNLib_checkIfLoginIsNeeded(JNIEnv* env, jobject obj)
{
	return CheckIfLoginIsNeeded();
}
//------------------------Recap--------------------------


//http://pickerwengs.blogspot.ca/2011/12/android-programming-objects-between.html
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getRecapList(JNIEnv* env, jobject obj)
{
	PRecapRowList *oList = getRecapList();    // get all
	if (oList&&oList->list_size() > 0)
	{
		string pld = oList->SerializeAsString();
		jbyteArray arr = ba_from_string(env, pld);

		delete oList;
		return arr;
	}
	if (oList != 0)
		delete oList;
	return NULL;
}



