/* sk_gps.h - GPS interface for the Sk_ application.
 *
 
 */

#ifndef _SK_NET__H_
#define _SK_NET__H_

#include <time.h>
#include "mkrnav.h"

#define NET_COMPRESS 0x00000001
#define NET_RESOLVED 0x00000002
#define TEST_NET_COMPRESS( flags ) ( sk_net_get_compress_enabled() ? (flags & NET_COMPRESS) : 0 )

#if defined (MKR_WIN32) && !defined (__SYMBIAN32__)

#include <winsock.h>

typedef SOCKET Sk_Socket; /* WIN32 style. */
#define SK_INVALID_SOCKET INVALID_SOCKET

#elif defined J2ME

struct sk_socket_t;
typedef struct sk_socket_t *Sk_Socket;
#define SK_INVALID_SOCKET ((Sk_Socket) NULL)

#elif defined __SYMBIAN32__

typedef void* Sk_Socket;
#define SK_INVALID_SOCKET NULL

#else

struct sk_socket_t;
typedef struct sk_socket_t *Sk_Socket;
#define SK_INVALID_SOCKET ((Sk_Socket) NULL)
//int sk_net_get_fd(Sk_Socket s);

#endif /* MKR_WIN32 */

#define SK_NET_IS_VALID(s) (s != SK_INVALID_SOCKET)

typedef void (*Sk_NetConnectCallback) (Sk_Socket socket, void *context, sk_result res);
typedef void (*Sk_NetSslConnectCallback) (Sk_Socket s, void *data, void *context, sk_result res);

Sk_Socket sk_net_connect(  const char*       protocol,
                                    const char*       name,
                                    time_t            update_time,
                                    int               default_port,
                                    int               flags,
                                    sk_result*   res); // Optional, can be NULL

// A-syncronious receive:
void *sk_net_connect_async (const char *protocol,
                                const char *name, 
                               const char *resolved_name,
                                time_t update_time,
                                int default_port,
                                int flags,
                                Sk_NetConnectCallback callback,
                                void *context);

/* If there is any problem detected, the 2 functions below MUST return
 * a negative value (never 0).
 */
int sk_net_receive (Sk_Socket s, void *data, int size);
int sk_net_send    (Sk_Socket s, const void *data, int length,
                         int wait);
int sk_net_send_async (Sk_Socket s, const void *data, int length);

Sk_Socket sk_net_listen(int port);
Sk_Socket sk_net_accept(Sk_Socket server_socket);

int sk_net_unique_id (unsigned char *buffer, unsigned int size);

void sk_net_close  (Sk_Socket s);

void sk_net_shutdown ( void );

void sk_net_initialize( void );

void sk_net_set_compress_enabled( BOOL value );
BOOL sk_net_get_compress_enabled( void );
int sk_net_get_fd(Sk_Socket s);
#endif // _SK_NET__H_

