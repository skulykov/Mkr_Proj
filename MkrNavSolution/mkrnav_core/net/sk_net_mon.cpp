/* sk_net_mon.c - network activity monitor

 *
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//#include "roadmap.h"
//#include "sk_lang.h"
//#include "sk_message.h"
//#include "sk_start.h"
#include "sk_main.h"
//#include "sk_start.h"
//#include "sk_screen.h"
//
//#include "sk_config.h"
#include "sk_net_mon.h"

#define ACTIVITY_TIMEOUT_SEC  3

static size_t RecvBytesCount;
static size_t SendBytesCount;
static int NumConnections;
static time_t LastActivityTime = 0;
static const char *LastErrorText = "";
//
static SK_NET_MON_STATE CurrentState = NET_MON_DISABLED;
//
//static Sk_ConfigDescriptor Sk_ConfigNetMonitorEnabled =
//                        SK_CONFIG_ITEM("Network", "Monitor Enabled");
static BOOL Sk_NetMonEnabled = FALSE;

static void periodic_callack (void) {

   if( Sk_NetMonEnabled == FALSE || (
#ifndef   _WIN32
      (CurrentState == NET_MON_IDLE) &&
#endif   // _WIN32
         ((time(NULL) - LastActivityTime) > ACTIVITY_TIMEOUT_SEC)) ) {

     // sk_message_unset('!');
      LastActivityTime = 0;
      sk_main_remove_periodic(periodic_callack);
      //sk_screen_redraw();
   }
}

// returns true iff we want to show monitor messages to the user
static BOOL show_net_mon(){
	return  Sk_NetMonEnabled;
}

static void update_activity (void) {
   if ( Sk_NetMonEnabled )
   {
      if (!LastActivityTime) sk_main_set_periodic (1000, periodic_callack);
      LastActivityTime = time(NULL);
   }
  
}


/* Called when a connection initialization is about to be attempted.
 * For mobile phones this should be called when a connection
 * module is started.
 */
void sk_net_mon_start (void) {

   /*assert (CurrentState == NET_MON_DISABLED || CurrentState == NET_MON_OFFLINE);
   CurrentState = NET_MON_START;
   if(show_net_mon())
   		sk_message_set('!', sk_lang_get("Accessing network..."));*/
   
   update_activity();
}


/*
 * Has to be called after the geo config in order to be able to determine
 * the proper default values for the net monitor ( Israel - true )
 *
 */
void sk_net_mon_initialize (void) {

   /*const char* netmon_enabled_default = sk_lang_rtl() ? "yes" : "no";
  
   sk_config_declare
      ( "user", &Sk_ConfigNetMonitorEnabled, netmon_enabled_default, NULL );
   Sk_NetMonEnabled = sk_config_match( &Sk_ConfigNetMonitorEnabled, "yes" );*/
}

/* Called after the connection module is shutdown
 */
void sk_net_mon_destroy (void) {
   //const char* netmon_cfg_value = Sk_NetMonEnabled ? "yes" : "no";
   //assert (CurrentState != NET_MON_DISABLED);
   //CurrentState = NET_MON_DISABLED;
   //// Network monitor enabled configuration value
   //sk_config_set( &Sk_ConfigNetMonitorEnabled, netmon_cfg_value );
   ////sk_screen_mark_redraw ();
}

void sk_net_mon_set_enabled( BOOL is_enabled )
{
   Sk_NetMonEnabled = is_enabled;
}

BOOL sk_net_mon_get_enabled( void )
{
   return Sk_NetMonEnabled;
}

/* Called when a new connection is about to be opened
 */
void sk_net_mon_connect (void) {
//   assert (CurrentState != NET_MON_DISABLED);
//   if ( CurrentState == NET_MON_OFFLINE ) { return; } //  connect means nothing in this case
//   CurrentState = NET_MON_CONNECT;
//   NumConnections++;
////   if(show_net_mon())
////  		 sk_message_set('!', sk_lang_get("Connecting..."));
//   update_activity();
}


/* Called after a connection is closed
 */
void sk_net_mon_disconnect (void) {
////   assert (CurrentState != NET_MON_DISABLED);
//   if ( CurrentState == NET_MON_OFFLINE || CurrentState == NET_MON_DISABLED ) { return; } //  disconnect means nothing in this case
//   assert (NumConnections);
//   NumConnections--;
//   if (NumConnections == 0) CurrentState = NET_MON_IDLE;
}


/* Called after data is sent over the net
 */
void sk_net_mon_send (size_t size) {
   //assert (CurrentState != NET_MON_DISABLED);
   //CurrentState = NET_MON_DATA;
   //SendBytesCount += size;
   //if(show_net_mon())
   //		sk_message_set('!', "%d KB", sk_net_mon_get_count() / 1024);
   //update_activity();
}


/* Called after data is received from the net
 */
void sk_net_mon_recv (size_t size) {
//
//#ifndef _WIN32
//   assert (CurrentState != NET_MON_DISABLED);
//#endif
//
//   CurrentState = NET_MON_DATA;
//   RecvBytesCount += size;
//   if(show_net_mon())
//   		sk_message_set('!', "%d KB", sk_net_mon_get_count() / 1024);
//   update_activity();
}


/* Called if an error occures
*/
void sk_net_mon_error (const char *text) {
   //LastErrorText = text;
   //CurrentState = NET_MON_ERROR;
   //if(show_net_mon())
  	//	 sk_message_set('!', text);
   //update_activity();
}


/* Returns the current status of the net activity
 */
SK_NET_MON_STATE sk_net_mon_get_status (void) {

   return CurrentState;
}


/* Returns the error message. Should be called only if we're in error state
 */
const char *sk_net_mon_get_error_text (void) {
   return LastErrorText;
}


/* Returns the total amount of bytes recvd+sent on the net
 */
size_t sk_net_mon_get_count (void) {
   return RecvBytesCount + SendBytesCount;
}

/* Called when user chose not to connect
 */
void sk_net_mon_offline (void) {
   /*CurrentState = NET_MON_OFFLINE;
   if(show_net_mon())
   		sk_message_set('!', sk_lang_get("Offline"));
   sk_start_exit ();*/
}



