#ifndef MKR_MKRMSGBOX_H_INCLUDED
#define MKR_MKRMSGBOX_H_INCLUDED

#include <jni.h>

extern "C" {

	JNIEXPORT void JNICALL Java_com_mkr_service_MkrMsgBox_InitMsgBoxNTV(JNIEnv * aJNIEnv, jobject aJObj);
	JNIEXPORT void JNICALL Java_com_mkr_service_MkrMsgBox_MsgBoxCallbackNTV
		(JNIEnv * aJNIEnv, jobject aJObj, jint aCbRes, jlong aCbContext);

};

void MkrMsgBox_DisposeRefs();
//---------------------------------------------------------------------------
typedef void(*MsgBoxOnCloseCallback)(int res_code, void* context);

/*
* JNI Callback context
*/
typedef struct
{
	MsgBoxOnCloseCallback callback;
	void *context;
} AndrMsgBoxCbContext;


/***********************************************************/
/*  Name        : void sk_androidmsgbox_show()
*  Purpose     : Shows android native message box
*  Params      :  title - title at the top of the box
*              :  message - text at the body of the box
*              :  label_ok - label of the ok button
*              :  label_cancel - label of the cancel button
*              :  context - context for the buttons' callback
*              :  callback - buttons callback
*
*/
void sk_androidmsgbox_show(const char* title, const char* message, const char* label_ok,
	const char* label_cancel, void* context, MsgBoxOnCloseCallback callback);

void test_msg_box();


#endif
