/* sk_main.c - The main function of the Sk_ application.

 */

#include <stdlib.h>
#include <string.h>

#ifdef SK_USES_GPE
#include <gpe/init.h>
#include <gpe/pixmaps.h>
#include <libdisplaymigration/displaymigration.h>
#endif

#include "mkrnav.h"
#include "filemng.h"
//#include "sk_start.h"
//#include "sk_config.h"
//#include "navigate/navigate_bar.h"
//#include "sk_canvas.h"
//#include "sk_history.h"
//#include "sk_keyboard.h"
//#include "sk_screen.h"
//#include "sk_gps.h"
//#include "sk_trip.h"
//#include "../editor/editor_main.h"
#include "sk_main.h"
//#include "sk_androidmain.h"
//#include "sk_androidcanvas.h"
//#include "sk_androidbrowser.h"
//#include "sk_androidspeechtt.h"
//#include "sk_messagebox.h"
#include "../JNI/MkrJNI.h"

#include "../jni/sk_android_msg.h"
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sk_sound.h>
#include "../tts/tts.h"

//#include "auto_hide_dlg.h"
//#include "sk_search.h"
//#include "sk_bar.h"
//#include "sk_device_events.h"
//#include "sk_androidmenu.h"
//#include "sk_androidogl.h"
//#include "sk_browser.h"
//#include "sk_speechtt.h"
// Crash handling
#include <signal.h>
//#include <sigcontext.h>
//#include "tts_was_provider.h"
//#include "unix/resolver.h"

int USING_PHONE_KEYPAD = 0;

unsigned char ANDR_APP_SHUTDOWN_FLAG = 0;	// The indicator if the application is in the exiting process

time_t timegm(struct tm *tm) { return 0; }

static BOOL sgFirstRun = FALSE;	// The indicator if current application run is after the upgrade (first execution)
static int sgHandledSignals[] = {SIGSEGV, SIGSTOP, SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSTKFLT};

static int sgAndroidBuildSdkVersion = -1;
static char sgAndroidDeviceName[128] = {0};
static char sgAndroidDeviceManufacturer[128] = {0};
static char sgAndroidDeviceModel[128] = {0};
static int sgAndroidAppMode = 0;

//======= IO section ========
typedef enum
{
	_IO_DIR_UNDEFINED = 0x0,
	_IO_DIR_CONNECT = 0x1,
	_IO_DIR_READ = 0x2,
	_IO_DIR_WRITE = 0x4,
} io_direction_type;

#define IO_VALID( ioId ) ( ( ioId ) >= 0 )
#define IO_INVALID_VAL (-1)

typedef struct sk_main_io {
   Sk_IO io;
   Sk_Input callback;

   int io_id;						// If is not valid < 0
   pthread_t handler_thread;
   pthread_mutex_t mutex;			// Mutex for the condition variable
   pthread_cond_t cond;				// Condition variable for the thread
   io_direction_type io_type;
   int pending_close;				// Indicator for the IO to be closed
   time_t start_time;
} sk_main_io;

/*
 * Kernel structure. Added in order to prevent includes problem
 *
 */
#if REDEFINITION_PROBLEM_IS_HERE
struct ucontext {
	unsigned long	  uc_flags;
	struct ucontext  *uc_link;
	stack_t		  uc_stack; 
	struct sigcontext uc_mcontext;
	sigset_t	  uc_sigmask;
	/* Allow for uc_sigmask growth.  Glibc uses a 1024-bit sigset_t.  */
	int		  unused[32 - (sizeof (sigset_t) / sizeof (int))];
	/* Last for extensibility.  Eight byte aligned because some
	   coprocessors require eight byte alignment.  */
 	unsigned long	  uc_regspace[128] __attribute__((__aligned__(8)));
};
#endif
typedef struct ucontext ucontext_t;

const struct timeval SOCKET_READ_SELECT_TIMEOUT = {30, 0}; // {sec, u sec}
const struct timeval SOCKET_WRITE_SELECT_TIMEOUT = {30, 0}; // {sec, u sec}
#define MESSAGE_DISPATCHER_TIMEOUT {10, 0} // {sec, nano sec}

#define CRASH_DUMP_ADDR_NUM		200		/* The number of addresses to dump when crash is occured */
#define CRASH_DUMP_ADDR_NUM_ON_SHUTDOWN      20      /* The number of addresses to dump when crash is occured on shutdown
                                                      * to not blow up the log on each exit
                                                     */
#define SK_MAX_IO 64
static struct sk_main_io Sk_MainIo[SK_MAX_IO];

static void sk_main_close_IO();
static void sk_main_reset_IO( sk_main_io *data );
static int sk_main_handler_post_wait( int ioMsg, sk_main_io *aIO );
static void sk_start_event (int event);
static void sk_main_signals_setup( void );
static void sk_main_termination_handler( int signum, siginfo_t *info, void *secret );
static void sk_main_dump_memory( volatile unsigned long *addr, int addr_num );
static const char *sk_main_get_signame( int sig );
//static EVirtualKey sk_main_back_btn_handler( void );
static void sk_main_handle_menu();
//static void on_device_event( device_event event, void* context );
static void sk_main_set_bottom_bar( BOOL force_hidden );
static BOOL sk_main_is_pending_close( sk_main_io *data );

extern void sk_androidrecommend_init( void );
extern void sk_androideditbox_init( void );
//===============================================================

typedef enum
{
	_KEY_NOT_HANDLED = 0,
	_KEY_HANDLED = 1
} key_handling_status_type;

//////// Timers section ////////////////
#define SK_MAX_TIMER 32

typedef struct sk_main_timer {
   int id;
   Sk_Callback callback;
}  sk_main_timer;
static sk_main_timer Sk_MainPeriodicTimer[SK_MAX_TIMER];

////////////////////////////////////////

static char *Sk_MainTitle = NULL;

static Sk_KeyInput Sk_MainInput = NULL;

static const int sgNavigateBarOffset = 50;


/*************************************************************************************************
 * sk_main_key_pressed()
 * Key pressed event handler. Android key codes mapping
 *
 */
int sk_main_key_pressed( int aKeyCode, int aIsSpecial, const char* aUtf8Bytes )
{

	//char regular_key[2] = {0};
	//EVirtualKey vk      = VK_None;
	//const char* pCode;

	//sk_log( SK_DEBUG, "Key event. Code %d, Is special %d, UTF8 : %s", aKeyCode, aIsSpecial, aUtf8Bytes );

	//// First of all handle virtual keys
	//switch( aKeyCode )
	//{
	//	case _andr_keycode_dpad_center	  :
	//	case _andr_keycode_enter		  :
	//	{
	//		regular_key[0] = ENTER_KEY;
	//		sk_keyboard_handler__key_pressed( regular_key, KEYBOARD_ASCII );
	//		return _KEY_HANDLED;
	//	}
	//	case _andr_keycode_del			  :
	//	{
	//		regular_key[0] = BACKSPACE;
	//		sk_keyboard_handler__key_pressed( regular_key, KEYBOARD_ASCII );
	//		return _KEY_HANDLED;
	//	}
	//	case _andr_keycode_call           : vk = VK_Call_Start;     break;
	//	case _andr_keycode_menu			  : sk_main_handle_menu(); break;
	//	case _andr_keycode_soft_left      : break;
	//	case _andr_keycode_home			  :	break;
	//	case _andr_keycode_return		  :
	//	case _andr_keycode_soft_right     :	{ vk = sk_main_back_btn_handler(); break; };
	//	case _andr_keycode_dpad_up        : vk = VK_Arrow_up;       break;
	//	case _andr_keycode_dpad_down      : vk = VK_Arrow_down;     break;
	//	case _andr_keycode_dpad_left      : vk = VK_Arrow_left;     break;
	//	case _andr_keycode_dpad_right     : vk = VK_Arrow_right;    break;
	//	case _andr_keycode_camera         : break;
	//	case _andr_keycode_search         : sk_search_menu();  break;
	//	case _andr_keycode_end_call       : sk_main_exit();     return _KEY_NOT_HANDLED;
	//	default:
	//		break;
	//}

	//// Handle virtual key if necessary
	//if( vk != VK_None )
	//{
	//	regular_key[0] = ( char ) ( vk & 0xFF );
	//	regular_key[1] = '\0';
	//	if ( sk_keyboard_handler__key_pressed( regular_key, KEYBOARD_VIRTUAL_KEY ) )
	//	{
	//		return _KEY_HANDLED;
	//	}
	//}
	//// Handle non-special keys (having utf8)
	//if ( !aIsSpecial )	// Not a special key
	//{
	//	// Handle regular keys. pCode - pointer to the null terminated bytes
	//	// of the utf character
	//	if( sk_keyboard_handler__key_pressed( aUtf8Bytes, KEYBOARD_ASCII ) )
	//	{
	//		return _KEY_HANDLED;
	//	}
	//}
	//// Any other case
	//return _KEY_NOT_HANDLED;
	return 1;
}

/*************************************************************************************************
 * void sk_main_android_os_build_sdk_version( int aVersion )
 * Sets the current os build sdk version
 *
 */
void sk_main_set_build_sdk_version( int aVersion )
{
	sgAndroidBuildSdkVersion = aVersion;
}

/*************************************************************************************************
 * int sk_main_set_build_sdk_version( void )
 * Gets the current os build sdk version
 *
 */
int sk_main_get_build_sdk_version( void )
{
	return sgAndroidBuildSdkVersion;
}

/*************************************************************************************************
 * BOOL sk_main_mtouch_supported( void )
 * Indicates if multitouch supported
 *
 */
BOOL sk_main_mtouch_supported( void )
{
   return TRUE;//( sgAndroidBuildSdkVersion >= ANDROID_OS_VER_ECLAIR );
}


/*************************************************************************************************
 * void sk_main_set_device_name( const char* device_name )
 * Sets the current device name
 *
 */
void sk_main_set_device_name( const char* device_name )
{
	strncpy( sgAndroidDeviceName, device_name, sizeof( sgAndroidDeviceName )-1 );
}

/*************************************************************************************************
 * void sk_main_set_device_model( const char* model )
 * Sets the current device model
 *
 */
void sk_main_set_device_model( const char* model )
{
   strncpy( sgAndroidDeviceModel, model, sizeof( sgAndroidDeviceModel )-1 );
}

/*************************************************************************************************
 * const char* sk_main_get_device_model( void )
 * Returns the device model name
 *
 */
const char* sk_main_get_device_model( void )
{
   return sgAndroidDeviceModel;
}


/*************************************************************************************************
 * void sk_main_set_device_manufacturer( const char* manufacturer )
 * Sets the current device manufacturer
 *
 */
void sk_main_set_device_manufacturer( const char* manufacturer )
{
   strncpy( sgAndroidDeviceManufacturer, manufacturer, sizeof( sgAndroidDeviceManufacturer )-1 );
}


/*************************************************************************************************
 * const char* sk_main_get_device_manufacturer( void )
 * Returns the device manufacturer
 *
 */
const char* sk_main_get_device_manufacturer( void )
{
   return sgAndroidDeviceManufacturer;
}


/*************************************************************************************************
 * const char* sk_main_get_device_name( void )
 * Returns the device name
 *
 */
const char* sk_main_get_device_name( void )
{
	return sgAndroidDeviceName;
}

/*************************************************************************************************
 * void sk_main_set_app_mode( int mode )
 * Sets the application mode
 *
 */
void sk_main_set_app_mode( int mode )
{
   sgAndroidAppMode = mode;
}

/*************************************************************************************************
 * int sk_main_is_widget_mode( void )
 * Returns the widget mode
 *
 */
int sk_main_is_widget_mode( void )
{
   return ( sgAndroidAppMode == 1 );
}

/*************************************************************************************************
 * void sk_main_show_contacts( void )
 * Requests the system to show the contacts activity
 *
 */
void sk_main_show_contacts( void )
{
   //FreeMapNativeManager_ShowContacts();
}

/*************************************************************************************************
 * void sk_main_show_contacts( void )
 * Requests the system to show the external browser with the requested url
 *
 */
void sk_main_open_url( const char* url )
{
   //FreeMapNativeManager_OpenExternalBrowser( url );
}
/*************************************************************************************************
 * int LogResult( int aVal, int aVerbose, const char *aStrPrefix)
 * Logs the system api call error if occurred
 * aVal - the tested call return value
 * aVerbose - the log severity
 * level, source and line are hidden in the SK_DEBUG, INFO etc macros
 */
int LogResult( int aVal, const char *aStrPrefix, int level, char *source, int line )
{
	if ( aVal == 0 )
		return 0;
	if ( aVal == -1 )
		aVal = errno;

	sk_log( level, source, line, "%s (Thread %d). Error %d: %s", aStrPrefix, pthread_self(), aVal, strerror( aVal ) );

	return aVal;
}


/*************************************************************************************************
 * sk_main_handler_post_wait( int ioMsg, sk_main_io *aIO )
 * Causes calling thread to wait on the condition variable for this IO
 * Returns error code
 */
static int sk_main_handler_post_wait( int ioMsg, sk_main_io *aIO )
{
	int waitRes, retVal = 0;
	struct timespec sleepWaitFor;
    struct timespec sleepTimeOut=MESSAGE_DISPATCHER_TIMEOUT;
	//sleepTimeOut = ( struct timespec ) MESSAGE_DISPATCHER_TIMEOUT;

    // Compute the timeout
	clock_gettime( CLOCK_REALTIME, &sleepWaitFor );
	sleepWaitFor.tv_sec += sleepTimeOut.tv_sec;
	sleepWaitFor.tv_nsec += sleepTimeOut.tv_nsec;

	retVal = pthread_mutex_lock( &aIO->mutex );
	if ( LogResult( retVal, "Mutex lock failed", SK_WARNING ) )
	{
		return retVal;
	}

	// POST THE MESSAGE TO THE MAIN LOOP
	// The main application thread has its message loop implemented
	// at the Java side calling JNI functions for handling
	// Supposed to be very small and efficient code.
	// Called inside the mutex to prevent signaling before waitin
	// TODO :: Check possibility to implement this message queue at the lower level
	// TODO :: Start POSIX thread as the main thread
	MkRlNativeManager_PostNativeMessage( ioMsg );

	// Waiting for the callback to finish
	retVal = pthread_cond_timedwait( &aIO->cond, &aIO->mutex, &sleepWaitFor );
	if ( LogResult( retVal, "Condition wait", SK_WARNING ) )
	{
		pthread_mutex_unlock( &aIO->mutex );
		return retVal;
	}

	retVal = pthread_mutex_unlock( &aIO->mutex );
	if ( LogResult( retVal, "Mutex unlock failed", SK_WARNING ) )
	{
		return retVal;
	}


	return retVal;
	
}

/*************************************************************************************************
 * BOOL sk_main_invalidate_pending_close( sk_main_io *data )
 * Thread safe, invalidating the IO pending for close
 * Returns TRUE if the IO was invalidated
 */
static BOOL sk_main_invalidate_pending_close( sk_main_io *data )
{
	BOOL res = FALSE;
	pthread_mutex_lock( &data->mutex );
	if ( data->pending_close )
	{
		data->pending_close = 0;
		res = TRUE;
	}
	pthread_mutex_unlock( &data->mutex );

	return res;
}

/*************************************************************************************************
 * BOOL sk_main_is_pending_close( sk_main_io *data )
 * Thread safe, reading the IO pending close
 * (Necessary for the non-atomic integer reads (M-Core cache implementations) and optimizer caching
 */
static BOOL sk_main_is_pending_close( sk_main_io *data )
{
	BOOL res = FALSE;
	pthread_mutex_lock( &data->mutex );
	res = data->pending_close;
	pthread_mutex_unlock( &data->mutex );

	return res;
}

/*************************************************************************************************
 * sk_main_socket_handler()
 * Socket handler thread body. Polling on the file descriptor.
 * ( Blocks with timeout )
 * Posts message to the main thread in case of file descriptor change
 * Waits on the condition variable before continue polling
 */
static void *sk_main_socket_handler( void* aParams )
{
	// IO data
	sk_main_io *data = (sk_main_io*) aParams;
	Sk_IO *io = &data->io;
	int io_id = data->io_id;
	// Sockets data
	fd_set fdSet;
	struct timeval selectReadTO = SOCKET_READ_SELECT_TIMEOUT;
	struct timeval selectWriteTO = SOCKET_WRITE_SELECT_TIMEOUT;
	int fd, retVal, ioMsg;
   const char *handler_dir;

	// Empty the set
	FD_ZERO( &fdSet );

	handler_dir = ( data->io_type & _IO_DIR_WRITE ) ? "WRITE" : "READ";
	sk_log( SK_INFO, "Starting the %s socket handler %d for socket %d IO ID %d", handler_dir, pthread_self(), io->os.socket, data->io_id );
	// Polling loop
	while( !sk_main_invalidate_pending_close( data ) &&
			IO_VALID( data->io_id ) &&
			( io->subsystem != SK_IO_INVALID ) &&
			!ANDR_APP_SHUTDOWN_FLAG )
	{
	   fd = sk_net_get_fd(io->os.socket);

		// Add the file descriptor to the set if necessary
		if ( !FD_ISSET( fd, &fdSet ) )
		{
			sk_log( SK_DEBUG, "Thread %d. Calling FD_SET for FD: %d", pthread_self(), fd );
			FD_SET( fd, &fdSet );
		}
		//selectTO = (struct timeval) SOCKET_SELECT_TIMEOUT;
		// Try to read or write from the file descriptor. fd + 1 is the max + 1 of the fd-s set!
		if ( data->io_type & _IO_DIR_WRITE )
		{
		   selectWriteTO = SOCKET_WRITE_SELECT_TIMEOUT;
			retVal = select( fd+1, NULL, &fdSet, NULL, &selectWriteTO );
			//sk_log( SK_DEBUG, "Thread %d. IO %d WRITE : %d. FD: %d", pthread_self(), data->io_id, retVal, fd );
		}
		else
		{
		   selectReadTO = SOCKET_READ_SELECT_TIMEOUT;
			retVal = select( fd+1, &fdSet, NULL, NULL, &selectReadTO );
			sk_log( SK_DEBUG, "Thread %d. IO %d READ : %d. FD: %d", pthread_self(), data->io_id, retVal, fd );
		}
		// Cancellation point - if IO is marked for invalidation - thread has to be closed
		if ( sk_main_invalidate_pending_close( data ) )
		{
			sk_log( SK_INFO, "IO %d invalidated. Thread %d going to exit...", io_id, pthread_self() );
			break;
		}

		if ( retVal == 0 )
		{
			sk_log( SK_ERROR, "IO %d Socket %d timeout", data->io_id, io->os.socket );
		}
		if( retVal  < 0 )
		{
			// Error in file descriptor polling
			sk_log( SK_ERROR, "IO %d Socket %d error for thread %d: Error # %d, %s", data->io_id, io->os.socket, pthread_self(), errno, strerror( errno ) );
			break;
		}
		/* Check if this input was unregistered while we were
		 * sleeping.
		 */
		if ( io->subsystem == SK_IO_INVALID || !IO_VALID( data->io_id ) )
		{
			break;
		}

		if ( retVal && !ANDR_APP_SHUTDOWN_FLAG ) // Non zero if data available
		{
			ioMsg = ( data->io_id & ANDROID_MSG_ID_MASK ) | ANDROID_MSG_CATEGORY_IO_CALLBACK;
			sk_log( SK_DEBUG, "Handling data for IO %d. Posting the message", data->io_id );
			// Waiting for the callback to finish its work
			if ( sk_main_handler_post_wait( ioMsg, data ) != 0 )
			{
				// The message dispatching is not performed !!!
				sk_log( SK_ERROR, "IO %d message dispatching failed. Thread %d going to finilize", data->io_id, pthread_self() );
				break;
			}
		}
    }
	sk_log( SK_INFO, "Finalizing the %s socket handler %d for socket %d IO ID %d", handler_dir, pthread_self(), io->os.socket, data->io_id );
	ioMsg = ( data->io_id & ANDROID_MSG_ID_MASK ) | ANDROID_MSG_CATEGORY_IO_CLOSE;
	MkRlNativeManager_PostNativeMessage( ioMsg );

	return (NULL);
}

/*************************************************************************************************
 * void sk_main_init_IO()
 * Initialization of the IO pool
 * void
 */
static void sk_main_init_IO()
{
	int i;
	int retVal;
	for( i = 0; i < SK_MAX_IO; ++i )
	{
		Sk_MainIo[i].io_id = IO_INVALID_VAL;
		retVal = pthread_mutex_init( &Sk_MainIo[i].mutex, NULL );
		LogResult( retVal, "Mutex init. ", SK_ERROR );
		retVal = pthread_cond_init( &Sk_MainIo[i].cond, NULL );
		LogResult( retVal, "Condition init init. ", SK_ERROR );
		Sk_MainIo[i].pending_close = 0;
    }
}


#define ROW_BUF_SIZE 64



/*************************************************************************************************
 * static void sk_main_dump_memory( volatile unsigned long *addr, int addr_num )
 * Dumps the memory starting from the provided address
 *
 */
static void sk_main_dump_memory( volatile unsigned long *addr, int addr_num )
{
	/*int i, cur_len = 0, available_size = 0;
	char row_buf[ROW_BUF_SIZE];
	volatile unsigned long *pAddr;

	sk_log_raw_data( "\n" );
	for(  i = 0; i < addr_num; ++i )
	{
		pAddr = addr + i;
		snprintf( row_buf, ROW_BUF_SIZE, "%08x : %08x\n", (unsigned int) pAddr, (unsigned int) *pAddr );
		sk_log_raw_data( row_buf );
	}*/
}


/*************************************************************************************************
 * const char *sk_main_get_signame( int sig )
 * Returns the literal interpretation of the signal number
 *
 */
static const char *sk_main_get_signame( int sig )
{
    switch(sig) {
    case SIGILL:     return "SIGILL";
    case SIGABRT:    return "SIGABRT";
    case SIGBUS:     return "SIGBUS";
    case SIGSTOP:    return "SIGSTOP";
    case SIGFPE:     return "SIGFPE";
    case SIGSEGV:    return "SIGSEGV";
    case SIGSTKFLT:  return "SIGSTKFLT";
    default:         return "?";
    }
}

/*************************************************************************************************
 * void sk_main_termination_handler( int signum, siginfo_t *info, void *secret )
 * The routine called by the signal. Prints the registers and stack. Gracefully exits the application
 *
 */
static void sk_main_termination_handler( int signum, siginfo_t *info, void *secret )
{
 //  static BOOL handling = FALSE;
	//ucontext_t *uc = (ucontext_t *)secret;
	//struct sigcontext* sig_ctx;
	//unsigned long sig_fault_addr = 0xFFFFFFFF;
	//int sig_errno = 0;
	//int sig_code = 0;
	//int i;
	//int addr_num = CRASH_DUMP_ADDR_NUM;

	//if ( handling )
	//   return;

	//handling = TRUE;

	//if ( info )
	//{
	//	 sig_fault_addr = (unsigned long) info->si_addr;
	//	 sig_errno = info->si_errno;
	//	 sig_code = info->si_code;
	//}

 //  /*
 //    * AGA NOTE:
 //    * Temprorary solution. Prints limited stack on exit crash because of consistent crashes on milestone 2.1
 //    * Should be removed after full analysis
 //    */
 //   if ( ANDR_APP_SHUTDOWN_FLAG )
 //   {
 //      sk_log_raw_data( "!!!! Crash on application shutdown !!!!!! \n" );
 //      addr_num = CRASH_DUMP_ADDR_NUM_ON_SHUTDOWN;
 //   }

	//sk_log( SK_ERROR, "Received signal %s %d. Fault address: %x. Error: %d. Code: %d  \n " \
	//		"********************************* STACK DUMP ******************************** ",
	//		sk_main_get_signame( signum ), signum, sig_fault_addr, sig_errno, sig_code );


	//if ( !uc )
	//{
	//	sk_log( SK_ERROR, "No context information in secret!" );
	//}
	//else
	//{
	//	sig_ctx = &uc->uc_mcontext;

	//	if ( sig_ctx )
	//	{
	//		sk_log_raw_data_fmt(
	//				"Registers: \n " \
	//				"R0 %08x  R1 %08x  R2 %08x  R3 %08x  R4 %08x  R5 %08x  R6 %08x\n" \
	//				"R7 %08x  R8 %08x  R9 %08x  R10 %08x\n" \
	//				"fp %08x  ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x  fault %08x\n " \
	//				"Stack data: Flags: %x, Size: %d, SP: %x",
	//				sig_ctx->arm_r0, sig_ctx->arm_r1, sig_ctx->arm_r2, sig_ctx->arm_r3, sig_ctx->arm_r4,
	//				sig_ctx->arm_r5, sig_ctx->arm_r6, sig_ctx->arm_r7, sig_ctx->arm_r8, sig_ctx->arm_r9,
	//				sig_ctx->arm_r10,
	//				sig_ctx->arm_fp, sig_ctx->arm_ip, sig_ctx->arm_sp, sig_ctx->arm_lr, sig_ctx->arm_pc,
	//				sig_ctx->arm_cpsr, sig_ctx->fault_address, uc->uc_stack.ss_flags, uc->uc_stack.ss_size, uc->uc_stack.ss_sp );

	//      sk_main_dump_memory( (volatile unsigned long*) sig_ctx->arm_sp, addr_num );
	//		fflush( NULL );
	//	}
	//	else
	//	{
	//		sk_log( SK_ERROR, "No signal context in secret!" );
	//	}

	//}
	//sk_main_exit();
}
/*************************************************************************************************
 * void sk_main_start_init()
 * Initialization of the application before start
 * void
 */
void sk_main_start_init()
{
	int i;

	sk_main_init_IO();

	//sk_start_subscribe ( sk_start_event );
	//tts_initialize();
	sk_main_signals_setup();

}

/*************************************************************************************************
 * void sk_main_signal_setup()
 * Sets up the signals for handling
 *
 */

static void sk_main_signals_setup( void )
{
	//int i;
	//int sig_count = sizeof( sgHandledSignals ) / sizeof( sgHandledSignals[0] );
	//struct sigaction new_action, old_action;

	///*
	// * Setting up the action params
	// */
 //   new_action.sa_sigaction = sk_main_termination_handler;
 //   sigemptyset (&new_action.sa_mask);
	// new_action.sa_flags = SA_SIGINFO;

	///*
	// * Set up the action for each signal in set
	// */
	//for ( i = 0; i < sig_count; ++i )
	//{
	//	sigaction( sgHandledSignals[i], &new_action, &old_action );
	//}
}


void sk_main_new ( const char *title, int width, int height )
{

	/*sk_canvas_new();

	editor_main_set (1);*/
}


void sk_main_set_keyboard ( struct Sk_FactoryKeyMap *bindings,
                                Sk_KeyInput callback ) {
   Sk_MainInput = callback;
}


void sk_main_add_separator (Sk_Menu menu)
{
   sk_main_add_menu_item (menu, NULL, NULL, NULL);
}


/*************************************************************************************************
 * sk_main_set_handler()
 * Creates the handler thread matching the io subsystem
 */
static void sk_main_set_handler( sk_main_io* aIO )
{
	Sk_IO *io = &aIO->io;
	int retVal = 0;

	// Handler threads for the IO
	switch ( io->subsystem )
	{
	   case SK_IO_SERIAL:
		   sk_log ( SK_ERROR, "Serial IO is sk_main_set_handlernot supported" );
		   retVal = 0;
		  break;
	   case SK_IO_NET:
       {
    	    const char *handler_dir;
    		handler_dir = ( aIO->io_type & _IO_DIR_WRITE ) ? "WRITE" : "READ";

		   retVal = pthread_create( &aIO->handler_thread, NULL,
										sk_main_socket_handler, aIO );

		   sk_log ( SK_DEBUG, "Creating handler thread for the net subsystem. ID: %d. Socket: %d. Thread: %d. Direction: %s",
													   aIO->io_id, aIO->io.os.socket, aIO->handler_thread, handler_dir );
		   break;
	   }

	   case SK_IO_FILE:
		   sk_log ( SK_ERROR, "FILE IO is not supported" );
		   retVal = 0;
		   break;
	}
	// Check the handler creation status
	if ( retVal != 0 )
	{
	   aIO->io_id = IO_INVALID_VAL;
	   sk_log ( SK_ERROR, "handler thread creation has failed with error # %d, %s", errno, strerror( errno ) );
	}
}
/*************************************************************************************************
 * sk_main_set_input()
 * Allocates the entry for the io and creates the handler thread
 */
void sk_main_set_input ( Sk_IO *io, Sk_Input callback )
{
	int i;
	int retVal;
	int fd;

	sk_log( SK_DEBUG, "Setting the input for the subsystem : %d\n", io->subsystem );

   if (io->subsystem == SK_IO_NET) fd = sk_net_get_fd(io->os.socket);
   else fd = io->os.file; /* All the same on UNIX except sockets. */

	for (i = 0; i < SK_MAX_IO; ++i)
	{
		if ( !IO_VALID( Sk_MainIo[i].io_id ) )
		{
			Sk_MainIo[i].io = *io;
			Sk_MainIo[i].callback = callback;
			Sk_MainIo[i].io_type = _IO_DIR_READ;
			Sk_MainIo[i].io_id = i;
			retVal = pthread_mutex_init( &Sk_MainIo[i].mutex, NULL );
         LogResult( retVal, "Mutex init. ", SK_ERROR );
         retVal = pthread_cond_init( &Sk_MainIo[i].cond, NULL );
         LogResult( retVal, "Condition init init. ", SK_ERROR );
			break;
		}
   }
   if ( i == SK_MAX_IO )
   {
      sk_log ( SK_FATAL, "Too many set input calls" );
      return;
   }

   // Setting the handler
   sk_main_set_handler( &Sk_MainIo[i] );
}


/*************************************************************************************************
 * sk_main_set_output()
 * Allocates the entry for the io and creates the handler thread
 */
void sk_main_set_output ( Sk_IO *io, Sk_Input callback, BOOL is_connect )
{

	int i, retVal;
	int fd;

	sk_log( SK_DEBUG, "Setting the output for the subsystem : %d\n", io->subsystem );

   if (io->subsystem == SK_IO_NET) fd = sk_net_get_fd(io->os.socket);
   else fd = io->os.file; /* All the same on UNIX except sockets. */

	for ( i = 0; i < SK_MAX_IO; ++i )
	{
		if ( !IO_VALID( Sk_MainIo[i].io_id ) )
		{
			Sk_MainIo[i].io = *io;
			Sk_MainIo[i].callback = callback;
			Sk_MainIo[i].io_id = i;
			Sk_MainIo[i].io_type = _IO_DIR_WRITE;

			if ( is_connect )
			   Sk_MainIo[i].io_type = (io_direction_type)(Sk_MainIo[i].io_type | _IO_DIR_CONNECT);

         Sk_MainIo[i].start_time = time(NULL);
         retVal = pthread_mutex_init( &Sk_MainIo[i].mutex, NULL );
         LogResult( retVal, "Mutex init. ", SK_ERROR );
         retVal = pthread_cond_init( &Sk_MainIo[i].cond, NULL );
         LogResult( retVal, "Condition init init. ", SK_ERROR );
			break;
		}
	}

	if ( i == SK_MAX_IO )
	{
	   sk_log ( SK_FATAL, "Too many set output calls" );
	   return;
	}

	// Setting the handler
	sk_main_set_handler( &Sk_MainIo[i] );

}


Sk_IO *sk_main_output_timedout(time_t timeout) {
   int i;

   for (i = 0; i < SK_MAX_IO; ++i) {
      if ( IO_VALID( Sk_MainIo[i].io_id ) &&
            ( Sk_MainIo[i].io_type & _IO_DIR_CONNECT ) )
      {
         if ( Sk_MainIo[i].start_time &&
               ( timeout > Sk_MainIo[i].start_time ) )
         {
            return &Sk_MainIo[i].io;
         }
      }
   }

   return NULL;
}


/*************************************************************************************************
 * sk_main_remove_input()
 * The IO entry is marked for deallocation. Will be available again when the thread will finish
 */
void sk_main_remove_input ( Sk_IO *io )
{
	int i;
	LOGI("sk_main_remove_input_1...");
	for (i = 0; i < SK_MAX_IO; ++i)
	{
	   if ( IO_VALID( Sk_MainIo[i].io_id ) && sk_io_same(&Sk_MainIo[i].io, io))
	   {
			 // Cancel the thread and set is valid to zero
 			 sk_log( SK_DEBUG, "Canceling IO # %d thread %d\n", i, Sk_MainIo[i].handler_thread );
			 pthread_mutex_lock( &Sk_MainIo[i].mutex );
			 Sk_MainIo[i].pending_close = 1;
			 Sk_MainIo[i].start_time = 0;
			 sk_io_invalidate( &Sk_MainIo[i].io );
			 pthread_mutex_unlock( &Sk_MainIo[i].mutex );

			 sk_log( SK_DEBUG, "Removing the input id %d for the subsystem : %d \n", i, io->subsystem );
			 break;
	   }
	}
	if ( i == SK_MAX_IO )
	{
	   sk_log ( SK_ERROR, "Can't find input to remove! System: %d. FD: %d", io->subsystem, io->os.file );
	}
}


/*************************************************************************************************
 * sk_main_reset_IO()
 * Resetting the IO entry fields
 */
static void sk_main_reset_IO( sk_main_io *data )
{
	sk_log( SK_DEBUG, "Reset IO: %d \n", data->io_id );

	data->callback = NULL;
	data->handler_thread = 0;
	sk_io_invalidate( &data->io );
	data->io_id = IO_INVALID_VAL;
	data->pending_close = 0;
   pthread_mutex_destroy( &data->mutex );
   pthread_cond_destroy( &data->cond );

}

/*************************************************************************************************
 * sk_main_timeout()
 * Called upon timeout expiration - calls the timer callback matching the received id
 */
static int sk_main_timeout ( int aTimerId )
{
	sk_main_timer lTimer;

	if( aTimerId < 0 || aTimerId > SK_MAX_TIMER )
		return FALSE;

	// Getting the appropriate timer
	lTimer = Sk_MainPeriodicTimer[aTimerId];

	Sk_Callback callback = ( Sk_Callback ) ( lTimer.callback );

	sk_log( SK_DEBUG, "Timer expired for timer : %d. Callback : %x\n", aTimerId, callback );

	if (callback != NULL)
	{
	   // Apply the callback
	  (*callback) ();
	}

	return TRUE;
}

/*************************************************************************************************
 * sk_main_set_periodic()
 * Allocates entry in the Timers table and schedules the new timer using the JNI layer
 *
 */
void sk_main_set_periodic (int interval, Sk_Callback callback)
{

   int index;
   int timerMessage; // Main loop message associated with this timer
   struct sk_main_timer *timer = NULL;

   // sk_log( SK_INFO, "Setting the periodic timer. Callback address : %x\n", callback );
   if ( ANDR_APP_SHUTDOWN_FLAG )
	   return;

   for (index = 0; index < SK_MAX_TIMER; ++index) {

      if (Sk_MainPeriodicTimer[index].callback == callback) {
         return;
      }
      if (timer == NULL) {
         if (Sk_MainPeriodicTimer[index].callback == NULL) {
            timer = Sk_MainPeriodicTimer + index;
            timer->id = index;
         }
      }
   }


   if (timer == NULL) {
      sk_log (SK_FATAL, "Timer table saturated");
   }

   // Update the structure
   timer->callback = callback;
   // Update the Android layer
   timerMessage = ( 0xFFFF & timer->id ) | ANDROID_MSG_CATEGORY_TIMER;
   MkRlNativeTimerManager_AddTask( timer->id, timerMessage, interval );

    sk_log( SK_DEBUG, "The timer %d is set up successfully with interval %d. Callback: %x \n", timer->id, interval,  timer->callback );
}

/*************************************************************************************************
 * sk_main_remove_periodic()
 * Deallocates timer and cancels it using the JNI layer
 */

void sk_main_remove_periodic (Sk_Callback callback) {

   int index;

   // sk_log( SK_DEBUG, "Removing the periodic timer. Callback address : %x\n", callback );

   for (index = 0; index < SK_MAX_TIMER; ++index) {

      if (Sk_MainPeriodicTimer[index].callback == callback) {

          Sk_MainPeriodicTimer[index].callback = NULL;
          // Update the Android layer - at this time it still be a call
          MkRlNativeTimerManager_RemoveTask( index );
          // sk_log( SK_DEBUG, "Timer %d with callback address : %x is removed", index, callback );

          return;
      }
   }
   sk_log (SK_ERROR, "timer 0x%08x not found", callback);

}


/*************************************************************************************************
 * sk_main_message_dispatcher()
 * Main thread messages dispatching routine. Calls the appropriate callback
 * according to the message information
 */
// Main loop message dispatcher
void sk_main_message_dispatcher( int aMsg )
{
	sk_log( SK_DEBUG, "Dispatching the message: %d", aMsg );
	// Dispatching process
	//sk_main_time_interval( 0 );
	if ( aMsg & ANDROID_MSG_CATEGORY_IO_CALLBACK )	// IO callback message type
	{
		int retVal;
		int indexIo = aMsg & ANDROID_MSG_ID_MASK;
		//sk_log( SK_DEBUG, "Dispatching the message for IO %d", indexIo );
		// Call the handler
		if ( IO_VALID( Sk_MainIo[indexIo].io_id ) && Sk_MainIo[indexIo].callback &&
				!sk_main_is_pending_close( &Sk_MainIo[indexIo] ) )
		{
			Sk_IO *io = &Sk_MainIo[indexIo].io;

			sk_log( SK_DEBUG, "Callback %x. IO %d", Sk_MainIo[indexIo].callback, Sk_MainIo[indexIo].io_id );
			Sk_MainIo[indexIo].callback( io );
 		    //sk_log( SK_DEBUG, "Callback %x. IO %d", Sk_MainIo[indexIo].callback, Sk_MainIo[indexIo].io_id );

			// Send the signal to the thread if the IO is valid
			sk_log( SK_INFO, "Signaling thread %d", Sk_MainIo[indexIo].handler_thread );
			retVal = pthread_mutex_lock( &Sk_MainIo[indexIo].mutex );
			LogResult( retVal, "Mutex lock failed", SK_WARNING );

			retVal = pthread_cond_signal( &Sk_MainIo[indexIo].cond );
			LogResult( retVal, "Condition wait", SK_INFO );

			pthread_mutex_unlock( &Sk_MainIo[indexIo].mutex );
			LogResult( retVal, "Condition unlock failed", SK_WARNING );
		}
		else
		{
			sk_log( SK_INFO, "The IO %d is undefined (Index: %d). Dispatching is impossible for message %d",Sk_MainIo[indexIo].io_id, indexIo, aMsg );
		}
	}
	// IO close message type - the thread was finalized
	if ( aMsg & ANDROID_MSG_CATEGORY_IO_CLOSE )
	{
		int indexIo = aMsg & ANDROID_MSG_ID_MASK;
		sk_main_reset_IO( &Sk_MainIo[indexIo] );
	}
	if ( aMsg & ANDROID_MSG_CATEGORY_TIMER )	// Timer message type
	{
		int indexTimer = aMsg & ANDROID_MSG_ID_MASK;
		//sk_log( SK_DEBUG, "Dispatching the message for Timer %d", indexTimer );
		// Handle timeout
		sk_main_timeout( indexTimer );
	}
	// Menu events message type
	if ( aMsg & ANDROID_MSG_CATEGORY_MENU )	// Timer message type
	{
		int itemId = aMsg & ANDROID_MSG_ID_MASK;
		// svk sk_androidmenu_handler( itemId );
	}
   // DNS resolver message
   if ( aMsg & ANDROID_MSG_CATEGORY_RESOLVER )
   {
      int itemId = aMsg & ANDROID_MSG_ID_MASK;
//      resolver_handler( itemId );
   }
}

void sk_main_set_status (const char *text) {}


void sk_main_flush (void)
{
	// Signal the java layer to update the screen
	/*FreeMapNativeManager_Flush();*/
}

/*************************************************************************************************
 * sk_main_exit()
 * Starts the shutdown process
 *
 */
void sk_main_exit (void)
{
   //if ( !ANDR_APP_SHUTDOWN_FLAG )
   //{
   //   sk_log( SK_WARNING, "Exiting the application\n" );

   //   ANDR_APP_SHUTDOWN_FLAG = 1;
   //   // Pass the control over the shutdown process to the java layer
   //   FreeMapNativeManager_ShutDown();
   //}
   /*
    * AGA NOTE: Not supposed to arrive to this point. We have System.exit() call in Java layer
    * Just in case that something was wrong we are here
    *
    */
   exit( 0 );
}
/*************************************************************************************************
 * sk_main_exit()
 * Starts the shutdown process
 *
 */
void sk_main_shutdown()
{
//	sk_log( SK_INFO, "Native shutdown start" );
//	// Free the resources
//	sk_start_exit ();
//
//	// Free the canvas resources
//	sk_log( SK_WARNING, "Closing the canvas" );
//
//#ifdef AGG
//	sk_canvas_agg_close();
//#endif
//
//	// Free the JNI resourses
//	sk_log( SK_WARNING, "Freeing the JNI objects" );
//	CloseJNIObjects();
//
//	sk_log( SK_WARNING, "Closing the IO" );
//
//	// Close the mutexes and conditions
//	sk_main_close_IO();
//
//	sk_log( SK_WARNING, "Native shutdown end" );
}


/*************************************************************************************************
 * int sk_main_time_interval( int aCallIndex )
 * For index 0 - returns the current time in milliseconds.
 * For index 1 - reutrns the interval from the last call with index 0
 *
 */
inline int sk_main_time_interval( int aCallIndex, int aPrintFlag )
{
	static struct timeval last_time = {0,-1};
	int delta = -1;
	struct timezone tz;
	struct timeval cur_time;
	// Get the current time
	if ( gettimeofday( &cur_time, &tz ) == -1 )
	{
		sk_log( SK_ERROR, "Error in obtaining current time\n" );
		return -1;
	}
	if ( aCallIndex == 0 )
	{
		last_time = cur_time;
		delta = 0;
		if ( aPrintFlag )
			sk_log( SK_WARNING, "Benchmark. Starting ... Thread: %d", pthread_self() );
	}
	if ( aCallIndex == 1 )
	{
		if ( last_time.tv_usec == - 1 )
			return 0;

		delta = ( cur_time.tv_sec * 1000 + cur_time.tv_usec / 1000 ) -
					( last_time.tv_sec * 1000 + last_time.tv_usec / 1000 );
		if ( aPrintFlag )
		{
			sk_log( SK_WARNING, "Benchmark. Interval: %d msec. Thread: %d", delta, pthread_self() );
//			if ( delta > 200 )
//				sk_log( SK_WARNING, "WARNING. PERFORMANCE LEAK!" );

		}
	}

	return delta;
}

/*************************************************************************************************
 * int sk_main_time_msec()
 * Returns time in millisec
 *
 *
 */
long sk_main_time_msec()
{
	static struct timeval last_time = {0,-1};
	struct timezone tz;
	struct timeval cur_time;
	long val;
	// Get the current time
	if ( gettimeofday( &cur_time, &tz ) == -1 )
	{
		sk_log( SK_ERROR, "Error in obtaining current time\n" );
		return -1;
	}
	val = ( cur_time.tv_sec * 1000 + cur_time.tv_usec / 1000 );
	return val;
}

/*************************************************************************************************
 * void sk_main_close_IO()
 * Deallocate IO associated resources
 *
 */
static void sk_main_close_IO()
{
	int i;
	int retVal;

	for (i = 0; i < SK_MAX_IO; ++i)
	{
		pthread_cond_destroy( &Sk_MainIo[i].cond );
		pthread_mutex_destroy( &Sk_MainIo[i].mutex );
    }
}

/*************************************************************************************************
 * void sk_main_minimize( void )
 * Minimize the window. Calls the Android layer through JNI
 *
 */
void sk_gui_minimize( void )
{
	//FreeMapNativeManager_MinimizeApplication( -1 );
}

/*************************************************************************************************
 * void sk_main_minimize( void )
 * Minimize the window. Calls the Android layer through JNI
 *
 */
void sk_gui_maximize( void )
{
	//FreeMapNativeManager_MaximizeApplication();
}
/*************************************************************************************************
 * void on_auto_hide_dialog_close( int exit_code, void* context )
 * Auto hide dialog on close callback
 *
 */
static void on_auto_hide_dialog_close( int exit_code, void* context )
{
}
/*************************************************************************************************
 * void sk_main_minimize( void )
 * Minimize button callback
 *
 */
void sk_main_minimize( void )
{

   //sk_start_about_exit();
   //auto_hide_dlg(on_auto_hide_dialog_close);
}

/*************************************************************************************************
 * EVirtualKey sk_main_back_btn_handler( void )
 * Handles the Android back button
 *
 */
//static EVirtualKey sk_main_back_btn_handler( void )
//{
//	EVirtualKey vk = VK_None;
//	if ( !ssd_dialog_is_currently_active() )
//	{
//      const char *focus;
//
//      focus = sk_trip_get_focus_name();
//      if ((focus &&  (!strcmp(focus, "GPS") || !strcmp(focus, "Location")) )){
//          sk_main_minimize();
//      }
//      else{
//         show_me_on_map();
//      }
//
//      if ( !sk_screen_refresh() )
//         sk_screen_redraw();
//	}
//	else
//	{
//		vk = VK_Softkey_right;
//	}
//	return vk;
//}


/*************************************************************************************************
 * void sk_start_event (int event)
 * Start event hanler
 *
 */
static void sk_start_event (int event) {
   /*switch (event) {
	   case SK_START_INIT:
	   {
	#ifdef FREEMAP_IL
		  editor_main_check_map ();
	#endif
		  sk_device_events_register( on_device_event, NULL);
		  sk_main_set_bottom_bar( TRUE );
		  sk_androidbrowser_init();
		  sk_androidrecommend_init();
		  sk_androideditbox_init();
		  sk_androidspeechtt_init();
		  tts_was_provider_init();
		  break;
	   }
   }*/
}


/*************************************************************************************************
 * static void sk_main_handle_menu()
 * Handle the android menu key
 *
 */
static void sk_main_handle_menu()
{
	/*if ( sk_bottom_bar_shown() )
	{
		sk_bottom_bar_hide();
	}
	else
	{
		sk_bottom_bar_show();
	}
	navigate_bar_resize();
	if ( !sk_screen_refresh() )
	   sk_screen_redraw();*/
}

/*************************************************************************************************
 * static void on_device_event( device_event event, void* context )
 * Device event handler
 *
 */

//static void on_device_event( device_event event, void* context )
//{
//   if( device_event_window_orientation_changed == event )
//   {
//	   sk_main_set_bottom_bar( FALSE );
//   }
//}

/*************************************************************************************************
 * static void sk_main_set_bottom_bar( void )
 * Sets the bottom bar depending on the screen orientation
 *
 */
static void sk_main_set_bottom_bar( BOOL force_hidden )
{
   /*if ( force_hidden )
   {
	   sk_bottom_bar_hide();
   }
   navigate_bar_resize();
   if ( !sk_screen_refresh() )
	   sk_screen_redraw();*/
}

void sk_main_set_first_run( BOOL value )
{
	sgFirstRun = value;
}

BOOL sk_horizontal_screen_orientation()
{
	return TRUE;//sk_canvas_is_landscape();
}

void sk_main_show_gps_disabled_warning()
{
	//sk_messagebox_custom( "Your phone's GPS is  turned OFF", "MkrNav requires GPS connection. Please turn on your GPS from Menu > Settings > Security & Location > Enable GPS satellites", 21, "#ffffff", 17, NULL );
	//sk_messagebox_timeout( "", 5 );
}

void sk_main_post_resolver_result( int entry_id )
{
   /*int msg = ( ANDROID_MSG_CATEGORY_RESOLVER | ( entry_id & ANDROID_MSG_ID_MASK ) );
   FreeMapNativeManager_PostNativeMessage ( msg );*/
}

Sk_Menu sk_main_new_menu (void) { return NULL; }


void sk_main_free_menu (Sk_Menu menu) {}


void sk_main_add_menu (Sk_Menu menu, const char *label) {}


void sk_main_add_menu_item (Sk_Menu menu,
                                 const char *label,
                                 const char *tip,
                                 Sk_Callback callback) {}


void sk_main_popup_menu (Sk_Menu menu, int x, int y) {}

void sk_main_set_window_size( void *w, int width, int height ) {}

void sk_main_toggle_full_screen (void) {}

void sk_main_add_tool (const char *label,
                            const char *icon,
                            const char *tip,
                            Sk_Callback callback) {}


void sk_main_add_tool_space (void) {}


void sk_main_add_canvas (void) {}


void sk_main_add_status (void)
{
// TODO:: Check the necessity
}

void sk_main_show (void) {}

void sk_main_set_cursor (int cursor) {}


//****************************************************************************
//http://man7.org/linux/man-pages/man2/timer_create.2.html   http://stackoverflow.com/questions/6274320/timer-in-android-jni?rq=1

#if HOW_TO_USE_TIMER_LINUX
#define CLOCKID CLOCK_REALTIME
       #define SIG SIGRTMIN

       #define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

       static void
       print_siginfo(siginfo_t *si)
       {
           timer_t *tidp = (timer_t *)si->si_value.sival_ptr;

           //printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
           //printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

           int v = timer_getoverrun(*tidp);
           if (v == -1)
		   {
               errExit("timer_getoverrun");
		   }
           else
		   {
               //printf("    overrun count = %d\n", or);
		   }
       }

       static void
       handler(int sig, siginfo_t *si, void *uc)
       {
           /* Note: calling printf() from a signal handler is not
              strictly correct, since printf() is not async-signal-safe;
              see signal(7) */

           /*printf("Caught signal %d\n", sig);*/
           print_siginfo(si);
           signal(sig, SIG_IGN);
       }

       int
       main(int argc, char *argv[])
       {
           timer_t timerid;
           struct sigevent sev;
           struct itimerspec its;
           long long freq_nanosecs;
           sigset_t mask;
           struct sigaction sa;

           if (argc != 3) {
               fprintf(stderr, "Usage: %s <sleep-secs> <freq-nanosecs>\n",
                       argv[0]);
               exit(EXIT_FAILURE);
           }

           /* Establish handler for timer signal */

           printf("Establishing handler for signal %d\n", SIG);
           sa.sa_flags = SA_SIGINFO;
           sa.sa_sigaction = handler;
           sigemptyset(&sa.sa_mask);
           if (sigaction(SIG, &sa, NULL) == -1)
               errExit("sigaction");

           /* Block timer signal temporarily */

           printf("Blocking signal %d\n", SIG);
           sigemptyset(&mask);
           sigaddset(&mask, SIG);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               errExit("sigprocmask");

           /* Create the timer */

           sev.sigev_notify = SIGEV_SIGNAL;
           sev.sigev_signo = SIG;
           sev.sigev_value.sival_ptr = &timerid;
           if (timer_create(CLOCKID, &sev, &timerid) == -1)
               errExit("timer_create");

           printf("timer ID is 0x%lx\n", (long) timerid);

           /* Start the timer */

           freq_nanosecs = atoll(argv[2]);
           its.it_value.tv_sec = freq_nanosecs / 1000000000;
           its.it_value.tv_nsec = freq_nanosecs % 1000000000;
           its.it_interval.tv_sec = its.it_value.tv_sec;
           its.it_interval.tv_nsec = its.it_value.tv_nsec;

           if (timer_settime(timerid, 0, &its, NULL) == -1)
                errExit("timer_settime");

           /* Sleep for a while; meanwhile, the timer may expire
              multiple times */

           printf("Sleeping for %d seconds\n", atoi(argv[1]));
           sleep(atoi(argv[1]));

           /* Unlock the timer signal, so that timer notification
              can be delivered */

           printf("Unblocking signal %d\n", SIG);
           if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
               errExit("sigprocmask");

           exit(EXIT_SUCCESS);
       }

#endif
