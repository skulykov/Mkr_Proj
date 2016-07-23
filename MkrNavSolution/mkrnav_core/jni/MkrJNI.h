/* MkrJNI.h - Interface for the JNI management module

 */
#ifndef INCLUDE__MKR_JNI__H
#define INCLUDE__MKR_JNI__H

#include <jni.h>


// JNI debug
#define JNI_DEBUG_ENABLED       1

#define JNI_LOG( ... )   {                  \
    if ( JNI_DEBUG_ENABLED )                \
        sk_log( __VA_ARGS__ );         \
}
// This structure represents the JNI object.
// The class and object references have to be destroyed manually
#define JNI_OBJ_NAME_LEN 		64
typedef struct
{
	jclass  obj_class;
	jobject obj;
	JavaVM* jvm;
	jint ver;
	char name[JNI_OBJ_NAME_LEN];
} android_jni_obj_type;

typedef struct
{
	JNIEnv*		env;
	jmethodID	mid;
} android_method_context_type;


// Pixel formats for the bitmap ( Reflection of the android SDK )
typedef enum
{
	_andr_pix_fmt_A_8   	=   	0x00000008,
	_andr_pix_fmt_JPEG  	=		0x00000100,
	_andr_pix_fmt_LA_88  	= 		0x0000000a,
	_andr_pix_fmt_L_8  	  	=	  	0x00000009,
	_andr_pix_fmt_OPAQUE  	=		0xffffffff,
	_andr_pix_fmt_RGBA_4444 =  	  	0x00000007,
	_andr_pix_fmt_RGBA_5551 =		0x00000006,
	_andr_pix_fmt_RGBA_8888 =		0x00000001,
	_andr_pix_fmt_RGBX_8888 =		0x00000002,
	_andr_pix_fmt_RGB_332  	=		0x0000000b,
	_andr_pix_fmt_RGB_565	=		0x00000004,
	_andr_pix_fmt_RGB_888	=		0x00000003,
	_andr_pix_fmt_TRANSLUCENT  	=	0xfffffffd,
	_andr_pix_fmt_TRANSPARENT  	=	0xfffffffe,
	_andr_pix_fmt_UNKNOWN		=	0x00000000,
	_andr_pix_fmt_YCbCr_420_SP	=	0x00000011,
	_andr_pix_fmt_YCbCr_422_SP	=	0x00000010
} android_pixel_format_type;


// Keycodes ( Reflection of the android SDK )
typedef enum
{
	_andr_keycode_soft_left			= 0x00000001,
	_andr_keycode_soft_right		= 0x00000002,
	_andr_keycode_home				= 0x00000003,
	_andr_keycode_return			= 0x00000004,
	_andr_keycode_call              = 0x00000005,
	_andr_keycode_end_call			= 0x00000006,
	_andr_keycode_dpad_up			= 0x00000013,
	_andr_keycode_dpad_down			= 0x00000014,
	_andr_keycode_dpad_left			= 0x00000015,
	_andr_keycode_dpad_right		= 0x00000016,
	_andr_keycode_dpad_center		= 0x00000017,
	_andr_keycode_camera            = 0x0000001B,
	_andr_keycode_enter				= 0x00000042,
	_andr_keycode_del				= 0x00000043,
	_andr_keycode_menu				= 0x00000052,
	_andr_keycode_search			= 0x00000054
} android_keycode_type;

// Actions for the keyboard
typedef enum
{
	_andr_ime_action_none 			= 0x00000001,
	_andr_ime_action_go 			   = 0x00000002,
	_andr_ime_action_search			= 0x00000003,
	_andr_ime_action_send			= 0x00000004,
	_andr_ime_action_next			= 0x00000005,
	_andr_ime_action_done 			= 0x00000006
} android_kb_action_type;

void InitJNIObject( android_jni_obj_type* aObj, JNIEnv * aJNIEnv, jobject aJObj, const char* aObjName );

jmethodID InitJNIMethodContext( android_jni_obj_type* aObj, android_method_context_type* aMthdContext,
																const char* aMethodName, const char* aMethodSig );

void DisposeJNIObject( android_jni_obj_type* aObj );

void DisposeCustomJNIObject( android_jni_obj_type* aObj, jobject aCustomObj );

void MkRlNativeTimerManager_AddTask( int aIndex, int aTimerMsg, int aInterval );

void MkRlNativeTimerManager_RemoveTask( int a_Index );

void MkRlNativeTimerManager_ShutDown();

void MkRlNativeManager_ShutDown();

void MkRlNativeManager_PostNativeMessage( int aMsg );

void MkRlNativeCanvas_RefreshScreen();

void MkRlSoundManager_PlayFile( const char* aFileName );

void MkRlNativeManager_SetBackLightOn( int aAlwaysOn );

void MkRlNativeManager_SetVolume( int aLvl, int aMinLvl, int aMaxLvl );

int MkRlNativeManager_getBatteryLevel();

void MkRlNativeManager_MinimizeApplication( int aDelay );

void MkRlNativeManager_MaximizeApplication( void );

void MkRlNativeManager_ShowDilerWindow();

void MkRlNativeManager_HideSoftKeyboard();

void MkRlNativeManager_ShowSoftKeyboard( int aAction, int aCloseOnAction );

int MkRlNativeManager_TakePicture( int aImageWidth, int aImageHeight, int aImageQuality,
                                                            char* aImageFolder, char* aImageFile );

int MkRlNativeManager_TakePictureAsync( int aImageWidth, int aImageHeight, int aImageQuality,
        char* aImageFolder, char* aImageFile );


int MkRlNativeManager_GetThumbnail( int aThumbWidth, int aThumbHeight, int bytePP, int* aBuf );

void MkRlSoundManager_LoadSoundData( char* aDataDir );

void MkRlNativeManager_SetIsMenuEnabled( int aEnabled );

void MkRlNativeManager_Flush();

void MkrMenuManager_AddOptionsMenuItem( int aItemId, const char* aLabel, const char* aIcon,
		int aIsNative, int aPortraitOrder, int aLandscapeOrder, int aItemType );

void MkRlNativeCanvas_RequestRender();

void MkRlNativeManager_SwapBuffersEgl();

int MkRlNativeManager_IsActive();

void MkRlNativeManager_HideWebView( void );

void MkRlNativeManager_ShowWebView( const char* aUrl, int aMinX, int aMinY, int aMaxX, int aMaxY, int aFlags );

void MkRlNativeManager_ShowEditBox( int aAction, int aStayOnAction, const char* aText, void* aContext, int aTopMargin, int aFlags );

void MkRlNativeManager_ResizeWebView( int aMinX, int aMinY, int aMaxX, int aMaxY );

void MkRlNativeManager_HideEditBox( void );

void MkRlNativeManager_LoadUrl( const char* aUrl );

void MkRlNativeManager_ShowContacts( void );

void MkRlNativeManager_MarketRate( void );

void MkrMsgBox_Show( const char* aTitle, const char* aMessage, const char* aLabelOk, const char* aLabelCancel, void* aCbContext );

void MkrSpeechttManager_StartNative( void* aContext, int aTimeout, const char* aLang, const char* aExtraPrompt, int aFlags );

void MkrSpeechttManager_Stop( void );

int MkrSpeechttManager_IsAvailable( void );

char* MkrResManager_LoadSkin( const char* aName, int* size );

void MkRlNativeManager_EditBoxCheckTypingLockCb( int aRes );

int MkrSoundRecorder_Start( const char* aPath, int aTimeout );

void MkrSoundRecorder_Stop( void );

void MkRlNativeManager_OpenExternalBrowser( const char* aUrl );

char** MkrResManager_LoadResList( const char* aPath );

void MkrAppWidgetManager_RouteRequestCallback(  int aStatus, const char* aErrDesc, const char* aDestDescription, int aTimeToDest );

void MkrAppWidgetManager_RequestRefresh( void );

void MkrMenuManager_ResetOptionsMenu( void );

void MkrMenuManager_SubmitOptionsMenu( void );

void MkRlTtsManager_Commit( const char* aText, const char* aFullPath, void* aContext );
void MkRlTtsManager_Prepare( void );
void MkRlTtsManager_Speak( const char* aText);

void MkRlNativeManager_DisposeRefs( void );
void MkRlNativeCanvas_DisposeRefs( void );
void MkRlSoundManager_DisposeRefs( void );
void MkRlNativeTimerManager_DisposeRefs( void );
void MkrMenuManager_DisposeRefs( void );
void MkrMsgBox_DisposeRefs( void );
void MkrSpeechttManager_DisposeRefs( void );
void MkrSoundRecorder_DisposeRefs( void );
void MkrResManager_DisposeRefs( void );
void MkRlTtsManager_DisposeRefs( void );
void MkrAppWidgetManager_DisposeRefs( void );
void CloseJNIObjects();


#endif // INCLUDE__FREEMAP_JNI__H
