/* sk_ssl.h - SSL implementation
 *
 */
 
#ifndef INCLUDE__SK__SSL__H
#define INCLUDE__SK__SSL__H

#include "sk_net.h"

struct sk_ssl_io_t;
typedef struct sk_ssl_io_t *Sk_SslIO;


int sk_ssl_read (void *context, void *buffer, int buffer_size);
int sk_ssl_send (Sk_SslIO io, const void *data, int length);
int sk_ssl_open (Sk_Socket s, void *data, Sk_NetSslConnectCallback callback);
void sk_ssl_close (void *context);

#endif /* INCLUDE__SK__SSL__H */
