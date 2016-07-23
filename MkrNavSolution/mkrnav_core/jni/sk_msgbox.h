/* sk_androidmsgbox.h - the interface to the android msgbox module
 
 */

#ifndef INCLUDE__SK_ANDROID_MSGBOX__H
#define INCLUDE__SK_ANDROID_MSGBOX__H

typedef void (*MsgBoxOnCloseCallback)( int res_code, void* context );

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
void sk_androidmsgbox_show( const char* title, const char* message, const char* label_ok,
      const char* label_cancel, void* context, MsgBoxOnCloseCallback callback );

void test_msg_box();

#endif // INCLUDE__SK_ANDROID_MSGBOX__H

