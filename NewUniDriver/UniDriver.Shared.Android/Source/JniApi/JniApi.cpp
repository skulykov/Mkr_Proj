#include "jniapi.h"
#include "jniobj.h"
#include "Dbg.h"
#include "../Network/NetTcp.h"
#include "../Network/NetComm.h"
#include <iostream>
#include <sstream>

#include "../mqtt_client/ClsMqtt.h"
#include "../BizLogic/hos/hos_interface.h"
#include "Global.h"
#include "../Network/tcp_client.h"

void JniApi_loginResp(int v);
void request_cb(void *o);

// The JNI object representing the current class
static android_jni_obj_type gJniObj;
//http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
/*For example, the Java method:
long f (int n, String s, int[] arr); 
has the following type signature:
(ILjava/lang/String;[I)J 
*/

#define JNI_CALL_MkrNLib_loginResp      "loginResp"
#define JNI_CALL_MkrNLib_loginResp_Sig  "(I)V"
#if 0
#define JNI_CALL_MkrNLib_ReceiveMsg      "ReceiveMsg"
#define JNI_CALL_MkrNLib_ReceiveMsg_Sig  "(Ljava/lang/String;Ljava/lang/String;)V"
#else
#define JNI_CALL_MkrNLib_ReceiveMsg      "ReceiveMsgCB"
#define JNI_CALL_MkrNLib_ReceiveMsg_Sig  "(II[B)V"
#endif
#define JNI_CALL_MkrNLib_AddNewUser "AddNewUser"

int init_signal_catch();

JNIEXPORT void JNICALL Java_com_mkr_service_MkrNLib_initOnStart(JNIEnv * env, jobject obj)
{
	//if (gInf)
	{
		//gInf->inititializeRenderingOnStart();
		LOGI("Start..JNI..");
		LOGI("Initializing JNI object MkrNLib");
		InitJNIObject(&gJniObj, env, obj, "MkrNLib");
		sk_log_to_file("Initializing JNI object MkrNLib\r\n");
		//init_signal_catch();
	}
	
}

/*************************************************************************************************
* MkrMsgBox_DisposeRefs()
* Dispose the JNI object of the MkrMsgBox module
*
*/
void JniApi_DisposeRefs()
{
	LOGI("Disposing the references for the JNI object %s", gJniObj.name);
	DisposeJNIObject(&gJniObj);
}

/*
//http://stackoverflow.com/questions/5198105/calling-a-java-method-from-c-in-android
JNIEXPORT jstring Java_com_mkr_service_MkrNLib_getJniString(JNIEnv* env, jobject obj)
{

	jstring jstr = env->NewStringUTF( "This comes from jni.");
	jclass clazz = env->FindClass( "com/mkr/unidriver/LoginActivity");
	jmethodID messageMe = env->GetMethodID( clazz, "messageMe", "(Ljava/lang/String;)Ljava/lang/String;");
	jobject result = env->CallObjectMethod(obj, messageMe, jstr);

	const char* str = env->GetStringUTFChars( (jstring)result, NULL); // should be released but what a heck, it's a tutorial :)
	LOGI("%s\n", str);//printf("%s\n", str);

	return env->NewStringUTF(str);
}
*/


//JNIEnv* gEnv = 0;
//jobject gObj;
int main_proto_read();
JNIEXPORT jint Java_com_mkr_service_MkrNLib_loginReq(JNIEnv* env, jobject obj,jstring user,jstring pass,jint rid)
{
//	main_proto_read();
	//gEnv = env;
	//gObj = obj;
	//jobject result = env->CallObjectMethod(obj, messageMe, jstr);
	
	const char* c_user = env->GetStringUTFChars((jstring)user, NULL); // should be released but what a heck, it's a tutorial :)
	const char* c_pass = env->GetStringUTFChars((jstring)pass, NULL);
	LOGI("%s %s\n", c_user,c_pass);//printf("%s\n", str);
	NetTcp::LoginRequest(std::string(c_user), std::string(c_pass), (int)rid,request_cb);
	
	env->ReleaseStringUTFChars(user, c_user);
	env->ReleaseStringUTFChars(pass, c_pass);

	//JniApi_loginResp(1); // just for test
	return 1;
}

void test_msg_box();
void request_cb(void *o)
{
	TcpRequestObj *obj = (TcpRequestObj *)o;
	if (obj != 0)
	{
		//ClsLoginMD *pL =(ClsLoginMD *) obj->parObj;  // this was dynamically created during login request !!!
		//if (pL != 0)
		//{
		//	string str = obj->GetResponse();
		//	if (!str.empty())
		//	{
		//		pL = pL->toJsonObj(str);
		//		if (pL != 0&&pL->res)
		//		{
		//			Global *g = Global::Get();
		//			g->SetLogin(pL);
		//			
		//			JniApi_loginResp(1);
		//			//test_msg_box();
		//		}
		//		else {
		//			JniApi_loginResp(0);
		//		}
		//	}
		//}
		LOGI("Got Login response..\r\n");
		string str = obj->GetResponse();
		if (!str.empty())
		{
			//LOGI("Got Login response..Not empty\r\n");
			ClsLoginMD lgn;
			lgn.ParseFromString(str);
			if (lgn.res())
						{
							Global *g = Global::Get();
							g->SetLogin(&lgn);
							
							JniApi_loginResp(2);

						}
						else {
							JniApi_loginResp(-2);
						}
		}
	}
}

void JniApi_loginResp(int v)
{	android_method_context_type lMthdContext;
	
	LOGI("Trying to call method %s through JNI \r\n", JNI_CALL_MkrNLib_loginResp);
	jmethodID mid = InitJNIMethodContext(&gJniObj, &lMthdContext, JNI_CALL_MkrNLib_loginResp,JNI_CALL_MkrNLib_loginResp_Sig);
	if (!mid || !lMthdContext.env)
	{
		LOGE("Failed to obtain method context!");
		return;
	}
	lMthdContext.env->CallVoidMethod(gJniObj.obj, lMthdContext.mid,v);

	
}

//===========================Logout====================================
JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_processLogout(JNIEnv * env, jobject obj)
{
	//EmployeeRow *emp = Global::Get()->GetCurrentProfile();
	/*if (emp != 0)
	{
		int clc = emp->cycle();
		string rv = Cycle2Str((HOSCYCLE)clc);
		return rv;
	}*/

	string driverId = Global::Get()->GetDriverID();
	EmployeeDB::Get()->SetLogoutById(driverId);
	LOGE("Logout .. %s",driverId.c_str());
	Global::Get()->SetCurrentProfile(NULL);   // clean up current profile
	
	ClsMqtt::Get()->stopMqtt();
	return 1;
}

//==============================Request================================
JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_sendRequest(JNIEnv * env, jobject obj, jint req)
{
	return SendRequest(req);
}
//=========================Mesaging====================================
JNIEXPORT void JNICALL Java_com_mkr_service_MkrNLib_initMQTT(JNIEnv * env, jobject obj)
{
	LOGI("doInitMqtt...");
	sk_log_to_file("doInitMqtt...\r\n");
	ClsMqtt::Get()->doInitMqtt(0, 0);

	MTcpClient::Get()->startTcpClient();
}

JNIEXPORT jint Java_com_mkr_service_MkrNLib_sendMessage(JNIEnv* env, jobject obj, jstring dest_user_id, jstring msg, jint flag)
{
	const char* c_dest = env->GetStringUTFChars((jstring)dest_user_id, NULL); // should be released but what a heck, it's a tutorial :)
	const char* c_msg = env->GetStringUTFChars((jstring)msg, NULL);
	LOGI("%s %s\n", c_dest, c_msg);
	ClsMqtt::Get()->add_server_message((char*)c_dest, (char*)c_msg, flag);

	env->ReleaseStringUTFChars(dest_user_id, c_dest);
	env->ReleaseStringUTFChars(msg, c_msg);
	return 1;
}
JNIEXPORT jint Java_com_mkr_service_MkrNLib_write2Log(JNIEnv* env, jobject obj, jstring msg)
{
	
	const char* c_msg = env->GetStringUTFChars((jstring)msg, NULL);
	int len = strlen(c_msg);
	if (len > 1022)
		len = 1022;
	write_to_log_file((void*)c_msg, strlen(c_msg));

	env->ReleaseStringUTFChars(msg, c_msg);
	return 1;
}
#if 0
void JniApi_PostReceivedMsg(const char *topic, const char *msg)
{
	android_method_context_type lMthdContext;
	JNIEnv* env;
	// !!! This method can be called from the another thread!!! It has to be attached to the VM
	gJniObj.jvm->AttachCurrentThread(&env, NULL);

	jmethodID mid = InitJNIMethodContext(&gJniObj, &lMthdContext, JNI_CALL_MkrNLib_ReceiveMsg,JNI_CALL_MkrNLib_ReceiveMsg_Sig);
	if (!mid || !lMthdContext.env)
	{
		LOGE("Failed to obtain method context!");
		return;
	}
	jstring _topic = lMthdContext.env->NewStringUTF( topic);
	jstring _msg = lMthdContext.env->NewStringUTF( msg);
	// Call the method
	lMthdContext.env->CallVoidMethod(gJniObj.obj, lMthdContext.mid, _topic,_msg);

	//// Release local references
	//if ( url )
	//   (*lMthdContext.env)->DeleteLocalRef( lMthdContext.env, url );

	
	lMthdContext.env->DeleteLocalRef(_topic);   // is this correct?? //http://stackoverflow.com/questions/6238785/newstringutf-and-freeing-memory
	lMthdContext.env->DeleteLocalRef(_msg);

	// Detach the thread
	gJniObj.jvm->DetachCurrentThread();
}
#else
void JniApi_PostReceivedMsgProto(int req,int par,void *payload, int payload_len)
{
	android_method_context_type lMthdContext;
	JNIEnv* env;
	// !!! This method can be called from the another thread!!! It has to be attached to the VM
	gJniObj.jvm->AttachCurrentThread(&env, NULL);

	jmethodID mid = InitJNIMethodContext(&gJniObj, &lMthdContext, JNI_CALL_MkrNLib_ReceiveMsg, JNI_CALL_MkrNLib_ReceiveMsg_Sig);
	if (!mid || !lMthdContext.env)
	{
		LOGE("Failed to obtain method context!");
		return;
	}
	jbyteArray arr = NULL;
	if (payload != NULL&&payload_len > 0)
	{
		arr = lMthdContext.env->NewByteArray(payload_len);
		lMthdContext.env->SetByteArrayRegion(arr, 0, payload_len, (jbyte*)payload);
	}
	// Call the method
	lMthdContext.env->CallVoidMethod(gJniObj.obj,lMthdContext.mid,req,par,arr);

	if(arr != NULL)
		lMthdContext.env->DeleteLocalRef(arr);   // is this correct?? //http://stackoverflow.com/questions/6238785/newstringutf-and-freeing-memory

	// Detach the thread
	gJniObj.jvm->DetachCurrentThread();
}
#endif
/*
void JniApi_PostAddNewUser(const char *did,const char *name)
{
	android_method_context_type lMthdContext;
	JNIEnv* env;
	// !!! This method can be called from the another thread!!! It has to be attached to the VM
	gJniObj.jvm->AttachCurrentThread(&env, NULL);

	jmethodID mid = InitJNIMethodContext(&gJniObj, &lMthdContext, JNI_CALL_MkrNLib_AddNewUser, JNI_CALL_MkrNLib_ReceiveMsg_Sig);
	if (!mid || !lMthdContext.env)
	{
		LOGE("Failed to obtain method context!");
		return;
	}
	jstring _did = lMthdContext.env->NewStringUTF(did);
	jstring _name = lMthdContext.env->NewStringUTF(name);
	// Call the method
	lMthdContext.env->CallVoidMethod(gJniObj.obj, lMthdContext.mid, _did, _name);

	//// Release local references
	//if ( url )
	//   (*lMthdContext.env)->DeleteLocalRef( lMthdContext.env, url );

	// Detach the thread
	gJniObj.jvm->DetachCurrentThread();
}
*/

//====================Send Byte array=====================================
//http://stackoverflow.com/questions/6246915/jni-passing-bytes-from-c-to-java
//Send_BA_From_Cpp
//string genGBReq();  // just for test
//JNIEXPORT jbyteArray Java_com_mkr_service_MkrNLib_getProtoMessage(JNIEnv* env, jobject obj, jint msg_id)
//{
//
//	string pld = genGBReq();
//
//	int size = pld.length();
//	//char* buffer = new char[size];
//	
//	jbyteArray arr = env->NewByteArray(size);
//	env->SetByteArrayRegion(arr, 0, size, (jbyte*)pld.c_str());
//
//	//delete[] buffer;
//	return arr;
//}

//====================Register=========================================
void register_request_cb(void *o)
{
	TcpRequestObj *obj = (TcpRequestObj *)o;
	if (obj != 0)
	{
		
		LOGI("Got Register response..\r\n");
		string str = obj->GetResponse();
		if (!str.empty())
		{
			//LOGI("Got Login response..Not empty\r\n");
			PEmployeeRow emp;
			emp.ParseFromString(str);
			if (!emp.username().empty())
			{
				Global *g = Global::Get();
				g->SetRegister(&emp);

				JniApi_loginResp(1);
				//test_msg_box();
			}
			else {
				JniApi_loginResp(-1);
			}
		}
	}
}

//http://stackoverflow.com/questions/3421300/passing-a-byte-in-java-to-a-function-in-c-through-jni-how-to-use-jarraybyte
JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_registerReq(JNIEnv *env, jobject obj, jbyteArray barray,jint msg_id)
{
	jbyte* bufferPtr = env->GetByteArrayElements( barray, NULL);
	jsize lengthOfArray = env->GetArrayLength( barray);

	/*PRegister rr;
	rr.ParseFromArray(bufferPtr, lengthOfArray);*/
	string payload = string((char*)bufferPtr, (int)lengthOfArray);
	NetTcp::RegisterRequest(payload, register_request_cb);

	//Finally, releasing it: 
	env->ReleaseByteArrayElements(barray, bufferPtr, 0);
	return 1;
}


JNIEXPORT jbyteArray JNICALL Java_com_mkr_service_MkrNLib_getVehicles(JNIEnv *env, jobject obj)
{
	jbyteArray arr = NULL;
	string DID = Global::Get()->GetDriverID();
	PBoxIdVehicleRowList* p = EmployeeDB::Get()->GetVehicleListFromDB(DID);
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
JNIEXPORT jint JNICALL Java_com_mkr_service_MkrNLib_saveVehicle(JNIEnv *env, jobject obj, jbyteArray ba)
{
	jbyte* bufferPtr = env->GetByteArrayElements(ba, NULL);
	jsize lengthOfArray = env->GetArrayLength(ba);

	PBoxIdVehicleRow vi;
	vi.ParseFromArray(bufferPtr, lengthOfArray);
	Global *g=Global::Get();
	string did = g->GetDriverID();
	vi.set_cid(g->GetCID());
	vi.set_mts(DateTime::UtcNow().getMillis());
	EmployeeDB::Get()->SaveVehicle2DB(&vi, did,false);
	env->ReleaseByteArrayElements(ba, bufferPtr, 0);
	return 1;
}

#if 0
//========================Catching Signal exceptions ================================
//#include <utils/CallStack.h>
//#include "corkscrew/backtrace.h"
#include <iomanip>

#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>

/*
void printStackTrace(unsigned int max_frames = 63)
{
	void* addrlist[max_frames + 1];

	// retrieve current stack addresses
	u32 addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

	if (addrlen == 0)
	{
		printf(stderr, "  <empty, possibly corrupt>\n");
		return;
	}

	char** symbollist = backtrace_symbols(addrlist, addrlen);

	for (u32 i = 3; i < addrlen; i++)
		printf(stderr, "%s\n", symbollist[i]) :
}
*/

//********************BackTrace****************************************************************
//http://stackoverflow.com/questions/8115192/android-ndk-getting-the-backtrace
struct BacktraceState
{
	void** current;
	void** end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
	BacktraceState* state = static_cast<BacktraceState*>(arg);
	uintptr_t pc = _Unwind_GetIP(context);
	if (pc) {
		if (state->current == state->end) {
			return _URC_END_OF_STACK;
		}
		else {
			*state->current++ = reinterpret_cast<void*>(pc);
		}
	}
	return _URC_NO_REASON;
}

size_t captureBacktrace(void** buffer, size_t max)
{
	BacktraceState state = { buffer, buffer + max };
	_Unwind_Backtrace(unwindCallback, &state);

	return state.current - buffer;
}

void dumpBacktrace(std::ostream& os, void** buffer, size_t count)
{
	for (size_t idx = 0; idx < count; ++idx) {
		const void* addr = buffer[idx];
		const char* symbol = "";

		Dl_info info;
		if (dladdr(addr, &info) && info.dli_sname) {
			symbol = info.dli_sname;
		}
		int status = 0;
		char *demangled = __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status);
		if(NULL != demangled && 0 == status)
		 os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol << " demangled  " << demangled << "\n";
		else
			os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol  << "\n";
	}
}
//*********************************************************************************************

//http://grokbase.com/t/gg/android-ndk/136jq58e5t/how-to-catch-fatal-signal-sigsegv-sigstkflt-etc
char sig_buffer[2048];
static void hdl(int sig, siginfo_t *siginfo, void *context)
{
	//!!!!IMPORTANT -> printf() can't be used in a signal handler
	//printf("Sending PID: %ld, UID: %ld\n",
	//	(long)siginfo->si_pid, (long)siginfo->si_uid);
	memset(sig_buffer, 0, 2048);
	sprintf(sig_buffer, "\r\nSignal: %d Sending PID: %ld, UID: %ld\r\n",sig,(long)siginfo->si_pid, (long)siginfo->si_uid);
	write_to_log_file(sig_buffer, strlen(sig_buffer));

	
	const size_t max = 30;
	void* buffer[max];
	std::ostringstream oss;

	dumpBacktrace(oss, buffer, captureBacktrace(buffer, max));
	//__android_log_print(ANDROID_LOG_INFO, "app_name", "%s", oss.str().c_str());

	sprintf(sig_buffer, "Stack: %s\r\n", oss.str().c_str());
	write_to_log_file(sig_buffer, strlen(sig_buffer));
}



int init_signal_catch()
{
	struct sigaction act;

	memset(&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &hdl;

	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &act, NULL);
	sigaction(SIGABRT, &act, NULL);


	return 0;
}
/*
one word of warning: you should not do memory allocation in the signal
handler. This includes using printf.

*/
#endif