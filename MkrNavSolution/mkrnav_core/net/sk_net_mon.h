/* sk_net_mon.h - Network activity monitor
 *
 
 */

#ifndef INCLUDE__SK__NET_MON__H
#define INCLUDE__SK__NET_MON__H

#include "mkrnav.h"

typedef enum tag_sk_net_mon_state{

   NET_MON_DISABLED,
   NET_MON_OFFLINE,
   NET_MON_START,
   NET_MON_IDLE,
   NET_MON_CONNECT,
   NET_MON_DATA,
   NET_MON_ERROR
	
} SK_NET_MON_STATE;

void sk_net_mon_start (void);
void sk_net_mon_connect (void);
void sk_net_mon_send (size_t size);
void sk_net_mon_recv (size_t size);
void sk_net_mon_disconnect (void);
void sk_net_mon_destroy (void);
void sk_net_mon_error (const char *text);
SK_NET_MON_STATE sk_net_mon_get_status (void);
size_t sk_net_mon_get_count (void);
void sk_net_mon_offline (void);
BOOL sk_net_mon_get_enabled( void );
void sk_net_mon_set_enabled( BOOL is_enabled );
void sk_net_mon_initialize (void);
#endif // INCLUDE__SK__NET_MON__H

