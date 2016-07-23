/* MKrNativeManager_JNI.c - JNI STUB for the MkRlNativeManager class
 
 */

#include "MKrNativeManager_JNI.h"
#include "MKrJNI.h"
#include "../main/sk_main.h"
#include "mkrnav.h"
//#include "ssd/ssd_dialog.h"
//#include "sk_editbox.h"
//#include "sk_androidmain.h"
//#include "sk_androidogl.h"
//#include "sk_urlscheme.h"
#include <string.h>
//#include "sk_browser.h"
//#include "WazeSpeechttManagerBase_JNI.h"
//#include "sk_keyboard.h"
//#include "sk_device_events.h"

#include <android\asset_manager_jni.h>


extern AAssetManager* assetMgr ;


// The JNI object representing the current class
static android_jni_obj_type gJniObj;

#define JNI_CALL_MkRlNativeManager_ShowEditBox      "ShowEditBox"
#define JNI_CALL_MkRlNativeManager_ShowEditBox_Sig     "(II[BJII)V"

#define JNI_CALL_MkRlNativeManager_HideEditBox      "HideEditBox"
#define JNI_CALL_MkRlNativeManager_HideEditBox_Sig     "()V"

#define JNI_CALL_MkRlNativeManager_ShowSoftKeyboard  		"ShowSoftKeyboard"
#define JNI_CALL_MkRlNativeManager_ShowSoftKeyboard_Sig 		"(II)V"

#define JNI_CALL_MkRlNativeManager_HideSoftKeyboard  		"HideSoftKeyboard"
#define JNI_CALL_MkRlNativeManager_HideSoftKeyboard_Sig 		"()V"

#define JNI_CALL_MkRlNativeManager_MarketRate      "MarketRate"
#define JNI_CALL_MkRlNativeManager_MarketRate_Sig     "()V"

#define JNI_CALL_MkRlNativeManager_ShowContacts        "ShowContacts"
#define JNI_CALL_MkRlNativeManager_ShowContacts_Sig       "()V"

#define JNI_CALL_MkRlNativeManager_ShutDown 			"ShutDown"
#define JNI_CALL_MkRlNativeManager_ShutDown_Sig 		"()V"

#define JNI_CALL_MkRlNativeManager_Flush 			"Flush"
#define JNI_CALL_MkRlNativeManager_Flush_Sig 		"()V"

#define JNI_CALL_MkRlNativeManager_getBatteryLevel 			"getBatteryLevel"
#define JNI_CALL_MkRlNativeManager_getBatteryLevel_Sig 		"()I"

#define JNI_CALL_MkRlNativeManager_SetBackLightOn 			"SetBackLightOn"
#define JNI_CALL_MkRlNativeManager_SetBackLightOn_Sig 		"(I)V"

#define JNI_CALL_MkRlNativeManager_SetIsMenuEnabled 			"SetIsMenuEnabled"
#define JNI_CALL_MkRlNativeManager_SetIsMenuEnabled_Sig 		"(I)V"

#define JNI_CALL_MkRlNativeManager_SetVolume 			"SetVolume"
#define JNI_CALL_MkRlNativeManager_SetVolume_Sig 		"(III)V"


#define JNI_CALL_MkRlNativeManager_PostNativeMessage 			"PostNativeMessage"
#define JNI_CALL_MkRlNativeManager_PostNativeMessage_Sig 		"(I)V"

#define JNI_CALL_MkRlNativeManager_MinimizeApplication 			"MinimizeApplication"
#define JNI_CALL_MkRlNativeManager_MinimizeApplication_Sig 		"(I)V"

#define JNI_CALL_MkRlNativeManager_MaximizeApplication 			"MaximizeApplication"
#define JNI_CALL_MkRlNativeManager_MaximizeApplication_Sig 		"()V"

#define JNI_CALL_MkRlNativeManager_ShowDilerWindow               "ShowDilerWindow"
#define JNI_CALL_MkRlNativeManager_ShowDilerWindow_Sig           "()V"

#define JNI_CALL_MkRlNativeManager_TakePicture               "TakePicture"
#define JNI_CALL_MkRlNativeManager_TakePicture_Sig           "(III[B[B)I"

#define JNI_CALL_MkRlNativeManager_TakePictureAsync               "TakePictureAsync"
#define JNI_CALL_MkRlNativeManager_TakePictureAsync_Sig           "(III[B[B)I"


#define JNI_CALL_MkRlNativeManager_GetThumbnail               "GetThumbnail"
#define JNI_CALL_MkRlNativeManager_GetThumbnail_Sig           "(II[I)I"

#define JNI_CALL_MkRlNativeManager_ShowWebView               "ShowWebView"
#define JNI_CALL_MkRlNativeManager_ShowWebView_Sig           "([BIIIII)V"

#define JNI_CALL_MkRlNativeManager_ResizeWebView               "ResizeWebView"
#define JNI_CALL_MkRlNativeManager_ResizeWebView_Sig           "(IIII)V"

#define JNI_CALL_MkRlNativeManager_HideWebView               "HideWebView"
#define JNI_CALL_MkRlNativeManager_HideWebView_Sig           "()V"

#define JNI_CALL_MkRlNativeManager_LoadUrl               "LoadUrl"
#define JNI_CALL_MkRlNativeManager_LoadUrl_Sig           "([B)V"

#define JNI_CALL_MkRlNativeManager_EditBoxCheckTypingLockCb               "EditBoxCheckTypingLockCb"
#define JNI_CALL_MkRlNativeManager_EditBoxCheckTypingLockCb_Sig           "(I)V"

#define JNI_CALL_MkRlNativeManager_OpenExternalBrowser               "OpenExternalBrowser"
#define JNI_CALL_MkRlNativeManager_OpenExternalBrowser_Sig           "(Ljava/lang/String;)V"



extern void sk_path_initialize( const char* aCfgPath );
extern void sk_device_backlight_monitor_reset( void );
extern void sk_main_set_first_run( BOOL value );
extern void sk_camera_take_picture_async_cb( int res );
/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_InitManagerNTV
 * Initializes the JNI object for the MkRlNativeManager class
 * Initializes the application resources path as provided from the Java layer
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_InitNativeManagerNTV
  ( JNIEnv* aJNIEnv, jobject aJObj, jstring aCfgPath, jint aBuildSdkVersion,
        jstring aDeviceName, jstring aDeviceModel, jstring aDeviceManufacturer )
{

	jboolean isCopy;
	const char* cfgPath = aJNIEnv->GetStringUTFChars( aCfgPath, &isCopy );
	const char* deviceName = aJNIEnv->GetStringUTFChars( aDeviceName, &isCopy );
	const char* deviceModel = aJNIEnv->GetStringUTFChars(  aDeviceModel, &isCopy );
	const char* deviceManufacturer = aJNIEnv->GetStringUTFChars( aDeviceManufacturer, &isCopy );

	sk_path_initialize( cfgPath );

	aJNIEnv->ReleaseStringUTFChars( aCfgPath, cfgPath );

	// The logger can be started only after the path initialization
	JNI_LOG( SK_INFO, "Initializing JNI object MkRlNativeManager" );
	InitJNIObject( &gJniObj, aJNIEnv, aJObj, "MkRlNativeManager" );

	/*sk_main_set_build_sdk_version( aBuildSdkVersion );

	sk_main_set_device_name( deviceName );
	sk_main_set_device_model( deviceModel );
	sk_main_set_device_manufacturer( deviceManufacturer );

   sk_log( SK_WARNING, "JNI LIB DEBUG: Version: %s, OS Version: %d. Device: %s. Model: %s. Manufacturer: %s. sk_start address: 0x%x.\n",
			sk_start_version(),
			aBuildSdkVersion,
			sk_main_get_device_name(),
			deviceModel,
			deviceManufacturer,
			sk_start );*/

   aJNIEnv->ReleaseStringUTFChars( aDeviceName, deviceName );
   aJNIEnv->ReleaseStringUTFChars( aDeviceModel, deviceModel );
   aJNIEnv->ReleaseStringUTFChars( aDeviceManufacturer, deviceManufacturer );
}


/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_AppStartNTV
 * Starts the application
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_AppStartNTV
  ( JNIEnv* aJNIEnv, jobject aJObj, jstring aUrl, int aAppMode )
{
	//sk_main_start_init();

	//const char* url = NULL;
	//if ( aUrl )
	//{
	//   char query[URL_MAX_LENGTH];
	//   jboolean isCopy;
	//	url = (*aJNIEnv)->GetStringUTFChars( aJNIEnv, aUrl, &isCopy );
	//	sk_urlscheme_remove_prefix( query, url );
 //     sk_urlscheme_init( query );
 //     (*aJNIEnv)->ReleaseStringUTFChars( aJNIEnv, aUrl, url );
	//}

 // sk_log( SK_WARNING, "Applicaiton started with URL string: %s", url );

 // sk_main_set_app_mode( aAppMode );

 // // Widget Mode
 // if ( aAppMode == 1 )
 // {
 //    sk_screen_set_background_run( TRUE );
 // }

 // sk_start(0, NULL);
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_AppShutDownNTV
 * Starts the Shutdown process on the native side
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_AppShutDownNTV
( JNIEnv* aJNIEnv, jobject aJObj )
{
	//sk_main_shutdown();
}

/*************************************************************************************************
 * MkRlNativeManager_ShutDown()
 * Calls the ExitManager function on the Java layer
 *
 */
void MkRlNativeManager_ShutDown()
{
	android_method_context_type lMthdContext;
	jmethodID mid;
	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShutDown );
	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShutDown,
													JNI_CALL_MkRlNativeManager_ShutDown_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	// Calling the method
	lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid );
}

/*************************************************************************************************
 * MkRlNativeManager_ShowContacts()
 * Shows the contacts list
 *
 */
void MkRlNativeManager_ShowContacts()
{
   android_method_context_type lMthdContext;
   jmethodID mid;
   JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShowContacts );
   mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShowContacts,
                                       JNI_CALL_MkRlNativeManager_ShowContacts_Sig );
   if ( !mid || !lMthdContext.env )
   {
      sk_log( SK_ERROR, "Failed to obtain method context!" );
      return;
   }

   // Calling the method
   lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid );
}


/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_UrlHandlerNTV
 * JNI wrapper for
 *
 */
JNIEXPORT jboolean JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_UrlHandlerNTV
( JNIEnv* aJNIEnv, jobject aJObj, jstring aUrl )
{
   jboolean isCopy;
   jboolean res = JNI_FALSE;
   //const char* url = (*aJNIEnv)->GetStringUTFChars( aJNIEnv, aUrl, &isCopy );

   ///*
   // * Try to handle by browser
   // */
   //sk_log( SK_INFO, "Processing url: %s", url );
   //if ( sk_browser_url_handler( url ) == TRUE )
   //{
   //   res = JNI_TRUE;
   //}

   ///*
   //  * Try to handle by url scheme
   //  */
   //if ( ( res == JNI_FALSE ) && sk_urlscheme_valid( url ) )
   //{
   //   char query[URL_MAX_LENGTH];
   //   sk_urlscheme_remove_prefix( query, url );
   //   sk_urlscheme_init( query );
   //   res = JNI_TRUE;
   //}

   //(*aJNIEnv)->ReleaseStringUTFChars( aJNIEnv, aUrl, url );

   return res;
}



/*************************************************************************************************
 * MkRlNativeManager_PostNativeMessage( int aMsg )
 * Calls the PostNativeMessage on the Java side to post the message to the main native thread
 *
 */
void MkRlNativeManager_PostNativeMessage( int aMsg )
{
	android_method_context_type lMthdContext;
	jmethodID mid;
	JNIEnv* env;
	JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_PostNativeMessage );
	// !!! This method can be called from the another thread!!! It has to be attached to the VM
	gJniObj.jvm->AttachCurrentThread( &env, NULL );

	mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_PostNativeMessage,
														JNI_CALL_MkRlNativeManager_PostNativeMessage_Sig );
	if ( !mid || !lMthdContext.env )
	{
		sk_log( SK_ERROR, "Failed to obtain method context!" );
		return;
	}

	// Calling the method
	lMthdContext.env->CallVoidMethod( gJniObj.obj, lMthdContext.mid, aMsg );
	// Detach the thread
	gJniObj.jvm->DetachCurrentThread(  );
}


/*************************************************************************************************
 * MkRlNativeManager_SetBackLightOn( int aAlwaysOn )
 * Sets the backlight to be always on or restores the system level
 *
 */
void MkRlNativeManager_SetBackLightOn( int aAlwaysOn )
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_SetBackLightOn );
	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_SetBackLightOn,
	//														JNI_CALL_MkRlNativeManager_SetBackLightOn_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aAlwaysOn );
}


/*************************************************************************************************
 * MkRlNativeManager_SetIsMenuEnabled( int aAlwaysOn )
 * Enables the menu
 *
 */
void MkRlNativeManager_SetIsMenuEnabled( int aEnabled )
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_SetIsMenuEnabled );
	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_SetIsMenuEnabled,
	//														JNI_CALL_MkRlNativeManager_SetIsMenuEnabled_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aEnabled );
}


/*************************************************************************************************
 * MkRlNativeManager_SetVolume( int aAlwaysOn )
 * Sets the new volume level for the media stream
 *
 */
void MkRlNativeManager_SetVolume( int aLvl, int aMinLvl, int aMaxLvl )
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_SetVolume );
	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_SetVolume,
	//														JNI_CALL_MkRlNativeManager_SetVolume_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aLvl, aMinLvl, aMaxLvl );
}


/*************************************************************************************************
 * void MkRlNativeManager_Flush()
 * Causes the main loop run
 *
 */
void MkRlNativeManager_Flush()
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNIEnv* env;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_Flush );

	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_Flush,
	//															JNI_CALL_MkRlNativeManager_Flush_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );

}

/*************************************************************************************************
 * int MkRlNativeManager_getBatteryLevel()
 * Returns the current battery level
 *
 */
int MkRlNativeManager_getBatteryLevel()
{
	int retVal=0;
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNIEnv* env;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_getBatteryLevel );

	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_getBatteryLevel,
	//														JNI_CALL_MkRlNativeManager_getBatteryLevel_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return 0;
	//}

	//// Calling the method
	//retVal = (*lMthdContext.env)->CallIntMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );

	return retVal;
}

/*************************************************************************************************
 * MkRlNativeManager_MinimizeApplication( int aDelay )
 * Minimizes the application and returns to the main window (if aDelay>= 0 ) after aDelay msecs
 *
 */
void MkRlNativeManager_MinimizeApplication( int aDelay )
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_MinimizeApplication );
	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_MinimizeApplication,
	//										JNI_CALL_MkRlNativeManager_MinimizeApplication_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aDelay );
}

/*************************************************************************************************
 * MkRlNativeManager_MaximizeApplication()
 * Minimizes the application from the background
 *
 */
void MkRlNativeManager_MaximizeApplication()
{
	//android_method_context_type lMthdContext;
	//jmethodID mid;
	//JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_MaximizeApplication );
	//mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_MaximizeApplication,
	//										JNI_CALL_MkRlNativeManager_MaximizeApplication_Sig );
	//if ( !mid || !lMthdContext.env )
	//{
	//	sk_log( SK_ERROR, "Failed to obtain method context!" );
	//	return;
	//}

	//// Calling the method
	//(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}

/*************************************************************************************************
 * MkRlNativeManager_ShowDilerWindow()
 * Shows the dialer window
 *
 */
void MkRlNativeManager_ShowDilerWindow()
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShowDilerWindow );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShowDilerWindow,
    //                                        JNI_CALL_MkRlNativeManager_ShowDilerWindow_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}

/*************************************************************************************************
 * MkRlNativeManager_ShowSoftKeyboard()
 * Shows the native keyboard
 *
 */
void MkRlNativeManager_ShowSoftKeyboard( int aAction, int aCloseOnAction )
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShowSoftKeyboard );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShowSoftKeyboard,
    //                                        JNI_CALL_MkRlNativeManager_ShowSoftKeyboard_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aAction, aCloseOnAction );
}

/*************************************************************************************************
 * MkRlNativeManager_ShowSoftKeyboard()
 * Hides the native keyboard
 *
 */
void MkRlNativeManager_HideSoftKeyboard()
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_HideSoftKeyboard );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_HideSoftKeyboard,
    //                                        JNI_CALL_MkRlNativeManager_HideSoftKeyboard_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}

/*************************************************************************************************
 * MkRlNativeManager_MarketRate()
 * Brings to front market rating window
 *
 */
void MkRlNativeManager_MarketRate( void )
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_MarketRate );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_MarketRate,
    //                                        JNI_CALL_MkRlNativeManager_MarketRate_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}

/*************************************************************************************************
 * MkRlNativeManager_ShowEditBox()
 * Shows the native edit box
 *
 */
void MkRlNativeManager_ShowEditBox( int aAction, int aStayOnAction, const char* aText, void* aContext, int aTopMargin, int aFlags )
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //jbyteArray text;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShowEditBox );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShowEditBox,
    //                                        JNI_CALL_MkRlNativeManager_ShowEditBox_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //text = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aText ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, text, 0, strlen( aText ), aText );

    //jlong ptrCtx = (jlong) aContext;

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //      aAction, aStayOnAction, text, ptrCtx, aTopMargin, aFlags );
}

/*************************************************************************************************
 * MkRlNativeManager_HideEditBox()
 * Hides the native edit box
 *
 */
void MkRlNativeManager_HideEditBox()
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_HideEditBox );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_HideEditBox,
    //                                        JNI_CALL_MkRlNativeManager_HideEditBox_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}


/*************************************************************************************************
 * MkRlNativeManager_TakePicture()
 * Shows the camera capture preview and saves the taken image
 *
 */
int MkRlNativeManager_TakePicture( int aImageWidth, int aImageHeight, int aImageQuality,
        char* aImageFolder, char* aImageFile )
{
    android_method_context_type lMthdContext;
    int retVal = -1;
    //jmethodID mid;
    //jbyteArray imageFolder, imageFile;
    //  JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_TakePicture );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_TakePicture,
    //                                        JNI_CALL_MkRlNativeManager_TakePicture_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return retVal;
    //}

    //imageFolder = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aImageFolder ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, imageFolder, 0, strlen( aImageFolder ), aImageFolder );

    //imageFile = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aImageFile ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, imageFile, 0, strlen( aImageFile ), aImageFile );

    //// Calling the method
    //retVal = (*lMthdContext.env)->CallIntMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        aImageWidth, aImageHeight, aImageQuality, imageFolder, imageFile );

    return retVal;
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_ConnectivityChangedNTV
 * Starts the application
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_ConnectivityChangedNTV
  ( JNIEnv* aJNIEnv, jobject aJObj, jboolean aConnected, jint aConnType, jstring aConnTypeStr )
{
  /* const char* conn_type_str = (*aJNIEnv)->GetStringUTFChars( aJNIEnv, aConnTypeStr, NULL );
   static int conn_state = -1;

   sk_log( SK_WARNING, "Android Connectivity Event. Connected: %d (Previous: %d). Network Type: %s ( Code: %d )",
         aConnected == JNI_TRUE, conn_state, conn_type_str, aConnType );

   if ( conn_state != (int) aConnected )
   {
      if ( aConnected == JNI_TRUE )
         sk_device_event_notification( device_event_network_connected );
      else
         sk_device_event_notification( device_event_network_disconnected );

      conn_state = ( int ) aConnected;
   }

   (*aJNIEnv)->ReleaseStringUTFChars( aJNIEnv, aConnTypeStr, conn_type_str );*/
}

/*************************************************************************************************
 * MkRlNativeManager_TakePictureAsync()
 * Shows the camera capture preview and saves the taken image
 *
 */
int MkRlNativeManager_TakePictureAsync( int aImageWidth, int aImageHeight, int aImageQuality,
        char* aImageFolder, char* aImageFile )
{
    android_method_context_type lMthdContext;
    int retVal = -1;
    //jmethodID mid;
    //jbyteArray imageFolder, imageFile;
    //  JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_TakePictureAsync );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_TakePictureAsync,
    //                                        JNI_CALL_MkRlNativeManager_TakePictureAsync_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return retVal;
    //}

    //imageFolder = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aImageFolder ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, imageFolder, 0, strlen( aImageFolder ), aImageFolder );

    //imageFile = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aImageFile ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, imageFile, 0, strlen( aImageFile ), aImageFile );

    //// Calling the method
    //retVal = (*lMthdContext.env)->CallIntMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        aImageWidth, aImageHeight, aImageQuality, imageFolder, imageFile  );

    return retVal;
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_TakePictureCallbackNTV
 * Callback for the asynchronous image capturing
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_TakePictureCallbackNTV
( JNIEnv* aJNIEnv, jobject aJObj, jint aRes )
{
   //sk_camera_take_picture_async_cb( aRes );
}

/*************************************************************************************************
 * MkRlNativeManager_GetThumbnail
 * Returns the thumbnail in the supplied byte buffer with the last taken image
 *
 */
int MkRlNativeManager_GetThumbnail( int aThumbWidth, int aThumbHeight, int bytePP, int* aBuf )
{
    android_method_context_type lMthdContext;
    int retVal = -1;
    //int buf_size;
    //jmethodID mid;
    //jintArray bufJNI;

    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_GetThumbnail );

    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_GetThumbnail,
    //                                        JNI_CALL_MkRlNativeManager_GetThumbnail_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return retVal;
    //}

    //buf_size = aThumbHeight*aThumbWidth;
    //bufJNI = (*lMthdContext.env)->NewIntArray( lMthdContext.env, buf_size );

    //// Calling the method
    //retVal = (*lMthdContext.env)->CallIntMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        aThumbWidth, aThumbHeight, bufJNI );

    //// Copy the elements
    //(*lMthdContext.env)->GetIntArrayRegion( lMthdContext.env, bufJNI, 0, buf_size, aBuf );

    return retVal;
}


/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_NativeMsgDispatcherNTV
 * JNI wrapper for the sk_main_message_dispatcher (will be called from the Java layer)
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_NativeMsgDispatcherNTV
( JNIEnv* aJNIEnv, jobject aJObj, jint aMsgId )
{
	// sk_main_message_dispatcher( aMsgId );
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_ShowGpsDisabledWarning
 * JNI wrapper for the sk_main_show_gps_disabled_warning (will be called from the Java layer)
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_ShowGpsDisabledWarningNTV
( JNIEnv* aJNIEnv, jobject aJObj )
{
	/*sk_main_show_gps_disabled_warning();*/
}
/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_BackLightMonitorResetNTV
 * JNI wrapper for the sk_device_backlight_monitor_reset (will be called from the Java layer)
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_BackLightMonitorResetNTV
( JNIEnv* aJNIEnv, jobject aJObj )
{
	/*sk_device_backlight_monitor_reset();*/
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_SetUpgradeRunNTV
 * JNI wrapper for the sk_main_set_first_run (will be called from the Java layer)
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_SetUpgradeRunNTV
( JNIEnv* aJNIEnv, jobject aJObj, jbyte aValue )
{
	/*sk_main_set_first_run( aValue );*/
}


/*************************************************************************************************
 * MkRlNativeManager_HideWebView
 * Hides android web view inside the dialog
 *
 */
void MkRlNativeManager_HideWebView( void )
{
   //android_method_context_type lMthdContext;
   //int retVal = -1;
   //jmethodID mid;

   //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_HideWebView );
   //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_HideWebView,
   //                                                      JNI_CALL_MkRlNativeManager_HideWebView_Sig );
   //if ( !mid || !lMthdContext.env )
   //{
   //    sk_log( SK_ERROR, "Failed to obtain method context!" );
   //    return;
   //}

   //   // Calling the method
   //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid );
}
/*************************************************************************************************
 * MkRlNativeManager_ShowWebView
 * Shows android web view inside the dialog
 *
 */
void MkRlNativeManager_ShowWebView( const char* aUrl, int aMinX, int aMinY, int aMaxX, int aMaxY, int aFlags )
{
    //android_method_context_type lMthdContext;
    //int retVal = -1;
    //jmethodID mid;
    //jbyteArray url;
    //  JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ShowWebView );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ShowWebView,
    //                                                                        JNI_CALL_MkRlNativeManager_ShowWebView_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //url = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aUrl ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, url, 0, strlen( aUrl ), aUrl );

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        url, aMinX, aMinY, aMaxX, aMaxY, aFlags );
}
/*************************************************************************************************
 * MkRlNativeManager_ResizeWebView
 * Resize android web view to fit the dialog
 *
 */
void MkRlNativeManager_ResizeWebView( int aMinX, int aMinY, int aMaxX, int aMaxY )
{
    //android_method_context_type lMthdContext;
    //int retVal = -1;
    //jmethodID mid;
    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_ResizeWebView );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_ResizeWebView,
    //                                                                        JNI_CALL_MkRlNativeManager_ResizeWebView_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        aMinX, aMinY, aMaxX, aMaxY );
}

/*************************************************************************************************
 * MkRlNativeManager_LoadUrl
 * Loads the url to the android web view
 *
 */
void MkRlNativeManager_LoadUrl( const char* aUrl )
{
    //android_method_context_type lMthdContext;
    //int retVal = -1;
    //jmethodID mid;
    //jbyteArray url;
    //  JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_LoadUrl );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_LoadUrl,
    //                                                                        JNI_CALL_MkRlNativeManager_LoadUrl_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //url = (*lMthdContext.env)->NewByteArray( lMthdContext.env, strlen( aUrl ) );
    //(*lMthdContext.env)->SetByteArrayRegion( lMthdContext.env, url, 0, strlen( aUrl ), aUrl );

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid,
    //        url );
}

/*************************************************************************************************
 * MkRlNativeManager_OpenExternalBrowser
 * Loads the url to the external browser
 *
 */
void MkRlNativeManager_OpenExternalBrowser( const char* aUrl )
{
    //android_method_context_type lMthdContext;
    //int retVal = -1;
    //jmethodID mid;
    //jstring url = NULL;

    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_OpenExternalBrowser );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_OpenExternalBrowser,
    //                                                                        JNI_CALL_MkRlNativeManager_OpenExternalBrowser_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}

    //if ( aUrl != NULL )
    //{
    //   url = (*lMthdContext.env)->NewStringUTF( lMthdContext.env, aUrl );
    //}

    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, url );

    //// Release local references
    //if ( url )
    //   (*lMthdContext.env)->DeleteLocalRef( lMthdContext.env, url );
}



/*************************************************************************************************
 * MkRlNativeManager_EditBoxCheckTypingLockCb
 * Callback for the check typing lock message box
 *
 */
void MkRlNativeManager_EditBoxCheckTypingLockCb( int aRes )
{
    //android_method_context_type lMthdContext;
    //jmethodID mid;

    //JNI_LOG( SK_INFO, "Trying to call method %s through JNI", JNI_CALL_MkRlNativeManager_EditBoxCheckTypingLockCb );
    //mid = InitJNIMethodContext( &gJniObj, &lMthdContext, JNI_CALL_MkRlNativeManager_EditBoxCheckTypingLockCb,
    //                                                               JNI_CALL_MkRlNativeManager_EditBoxCheckTypingLockCb_Sig );
    //if ( !mid || !lMthdContext.env )
    //{
    //    sk_log( SK_ERROR, "Failed to obtain method context!" );
    //    return;
    //}


    //// Calling the method
    //(*lMthdContext.env)->CallVoidMethod( lMthdContext.env, gJniObj.obj, lMthdContext.mid, aRes );
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_SetBackgroundRunNTV
 * JNI wrapper for the sk_main_set_first_run (will be called from the Java layer)
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_SetBackgroundRunNTV
( JNIEnv* aJNIEnv, jobject aJObj, jint aValue )
{
	/*if ( aValue )
	{
		sk_start_app_run_bg();
	}
	else
	{
		sk_start_app_run_fg();
	}*/
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_EditBoxCallbackNTV
 * Edit box callback
 *
 */
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_EditBoxCallbackNTV
 ( JNIEnv* aJNIEnv, jobject aJObj, jint aRes, jstring aText, jlong aCbContext )
 {
    /*EditBoxContextType *ctx = ( EditBoxContextType* ) aCbContext;
    if ( aRes > 0 )
    {
       jboolean isCopy;
       const char* text = (*aJNIEnv)->GetStringUTFChars( aJNIEnv, aText, &isCopy );

       ctx->callback( dec_ok, text, ctx->cb_context );

       (*aJNIEnv)->ReleaseStringUTFChars( aJNIEnv, aText, text );
    }
    else
    {
       sk_editbox_confirm_warning_hide();
       ctx->callback( dec_cancel, "", ctx->cb_context );
    }
    sk_editbox_dlg_hide();

    free( ctx );*/
}

/*************************************************************************************************
 * Java_com_mkr_navgl_service_MkRlNativeManager_EditBoxCheckTypingLockNTV
 * Checks if the editbox typing lock is enabled
 *
 */
JNIEXPORT jboolean JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_EditBoxCheckTypingLockNTV
   ( JNIEnv *aJNIEnv, jobject aJObj )
 {
    jboolean res = JNI_FALSE;
    /*if ( sk_keyboard_typing_locked( TRUE ) )
    {
       res = JNI_TRUE;
    }  */  
    return res;
 }
//------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_mkr_navgl_service_MkRlNativeManager_createAssetManager(JNIEnv* env, jobject aJObj, jobject assetManager)
 {
	 AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
	 assert(NULL != mgr);
	 assetMgr = mgr;
	 env->NewGlobalRef(assetManager); //won't be garbage collected.
 }
/*************************************************************************************************
 * MkRlNativeManager_DisposeRefs()
 * Dispose the JNI object of the MkRlNativeManager module
 *
 */
void MkRlNativeManager_DisposeRefs()
{
    JNI_LOG( SK_INFO, "Disposing the references for the JNI object %s", gJniObj.name );
	DisposeJNIObject( &gJniObj );
}



