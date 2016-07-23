/* sk_androidmsgbox
 *
 */

#include "sk_msgbox.h"
#include "MkrJNI.h"
#include "../core/mkrnav.h"
//#include "sk_screen.h"
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
      const char* label_cancel, void* context, MsgBoxOnCloseCallback callback )
{
  /* if ( sk_screen_get_background_run() )
      return;
*/
   AndrMsgBoxCbContext* msg_box_ctx =(AndrMsgBoxCbContext* ) malloc( sizeof( AndrMsgBoxCbContext ) );

   msg_box_ctx->callback = callback;
   msg_box_ctx->context = context;

   MkrMsgBox_Show( title, message, label_ok, label_cancel, msg_box_ctx );
}

static void on_terms_callback( int res_code, void *context )
{
   if ( res_code == 1/*dec_ok*/ )
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
	sk_androidmsgbox_show("Terms of use", "Test", "Accept", "Decline", NULL, on_terms_callback );
}