#include "MkrMsgBox.h"
#include "JniObj.h"
#include <string.h>
#include "Dbg.h"

// The JNI object representing the current class
static android_jni_obj_type gJniObj;


#define JNI_CALL_MkrMsgBox_Show      "Show"
#define JNI_CALL_MkrMsgBox_Show_Sig  "([B[B[B[BJ)V"


/*************************************************************************************************
* Java_com_waze_MkrMsgBox_InitMsgBoxNTV()
* Initializes the JNI environment object for the MkrMsgBox class
*
*/
JNIEXPORT void JNICALL Java_com_mkr_service_MkrMsgBox_InitMsgBoxNTV
(JNIEnv * aJNIEnv, jobject aJObj)
{
	LOGI("Initializing JNI object MkrMsgBox");
	// JNI object initialization
	InitJNIObject(&gJniObj, aJNIEnv, aJObj, "MkrMsgBox");
}

/*************************************************************************************************
* Java_com_waze_MkrMsgBox_MsgBoxCallbackNTV()
* Message box callback
*
*/
JNIEXPORT void JNICALL Java_com_mkr_service_MkrMsgBox_MsgBoxCallbackNTV
(JNIEnv * aJNIEnv, jobject aJObj, jint aCbRes, jlong aCbContext)
{

	AndrMsgBoxCbContext* ctx = (AndrMsgBoxCbContext*)aCbContext;
	//sk_log(SK_DEBUG, "Calling the callback 0x%x for the result: %d", ctx->callback, aCbRes);
	if (ctx->callback)
	{
		// Be aware of the 1 is always "Ok" button result
		ctx->callback(aCbRes, ctx->context);
	}
	free(ctx);
}
/*************************************************************************************************
* MkrMsgBox_Show()
* Shows native messagebox
* aTitle - message box title
* aMessage - message text
* aLabelOk - label for Ok button
* aLabelCancel - label for Cancel button
* aCbContext - callback context
*/
void MkrMsgBox_Show(const char* aTitle, const char* aMessage, const char* aLabelOk, const char* aLabelCancel, void* aCbContext)
{
	JNIEnv *env;
	jmethodID mid;
	android_method_context_type lMthdContext;
	jbyteArray title = NULL;
	jbyteArray message = NULL;
	jbyteArray label_ok = NULL;
	jbyteArray label_cancel = NULL;

	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkrMsgBox_Show );
	LOGI("Trying to call method %s through JNI \r\n", JNI_CALL_MkrMsgBox_Show);
	mid = InitJNIMethodContext(&gJniObj, &lMthdContext, JNI_CALL_MkrMsgBox_Show,
		JNI_CALL_MkrMsgBox_Show_Sig);
	if (!mid || !lMthdContext.env)
	{
		LOGE("Failed to obtain method context!");
		return;
	}

	if (aTitle != NULL)
	{
		title = lMthdContext.env->NewByteArray(strlen(aTitle));
		lMthdContext.env->SetByteArrayRegion(title, 0, strlen(aTitle), (const jbyte *)aTitle);
	}
	if (aMessage != NULL)
	{
		message = lMthdContext.env->NewByteArray(strlen(aMessage));
		lMthdContext.env->SetByteArrayRegion(message, 0, strlen(aMessage), (const jbyte *)aMessage);
	}
	if (aLabelOk != NULL)
	{
		label_ok = lMthdContext.env->NewByteArray(strlen(aLabelOk));
		lMthdContext.env->SetByteArrayRegion(label_ok, 0, strlen(aLabelOk), (const jbyte *)aLabelOk);
	}
	if (aLabelCancel != NULL)
	{
		label_cancel = lMthdContext.env->NewByteArray(strlen(aLabelCancel));
		lMthdContext.env->SetByteArrayRegion(label_cancel, 0, strlen(aLabelCancel), (const jbyte *)aLabelCancel);
	}

	// Call the method
	lMthdContext.env->CallVoidMethod(gJniObj.obj, lMthdContext.mid,
		title, message, label_ok, label_cancel, (jlong)aCbContext);
}

/*************************************************************************************************
* MkrMsgBox_DisposeRefs()
* Dispose the JNI object of the MkrMsgBox module
*
*/
void MkrMsgBox_DisposeRefs()
{
	LOGI( "Disposing the references for the JNI object %s", gJniObj.name);
	DisposeJNIObject(&gJniObj);
}

//-------------------------------------------------------
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
	const char* label_cancel, void* context, MsgBoxOnCloseCallback callback)
{
	/* if ( sk_screen_get_background_run() )
	return;
	*/
	AndrMsgBoxCbContext* msg_box_ctx = (AndrMsgBoxCbContext*)malloc(sizeof(AndrMsgBoxCbContext));

	msg_box_ctx->callback = callback;
	msg_box_ctx->context = context;

	MkrMsgBox_Show(title, message, label_ok, label_cancel, msg_box_ctx);
}

static void on_terms_callback(int res_code, void *context)
{
	if (res_code == 1/*dec_ok*/)
	{
		//on_terms_accepted();
	}
	else
	{
		//on_terms_declined();
	}
}

void test_msg_box()
{
	//roadmap_androidmsgbox_show( roadmap_lang_get ("Terms of use"), &text[0], roadmap_lang_get("Accept"), roadmap_lang_get("Decline"), NULL, on_terms_callback );
	sk_androidmsgbox_show("Terms of use", "Test", "Accept", "Decline", NULL, on_terms_callback);
}

