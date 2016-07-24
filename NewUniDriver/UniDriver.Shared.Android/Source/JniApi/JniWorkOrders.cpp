#include "JniWorkOrders.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include <iostream>

#include "../mqtt_client/ClsMqtt.h"
#include "../BizLogic/workorders/workorders_interface.h"
#include "Global.h"
#include "WorkOrderDB.h"

//===========================WorkOrdersActivity======================================================================================
JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getWOList(JNIEnv* env, jobject obj)
{
	jbyteArray arr = NULL;
	PWorkOrderList* p = WorkOrderDB::Get()->GetAllWorkOrders(); 
	if (p != NULL)
	{
		string pld = p->SerializeAsString();
		int size = pld.length();
		arr = env->NewByteArray(size);
		env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
		
		delete p;
	}
	return arr;

}


