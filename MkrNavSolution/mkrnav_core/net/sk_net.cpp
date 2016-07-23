/* sk_net.c - Network interface for the MkrNav application.
 *
 *
 * SYNOPSYS:
 *
 *   See sk_net.h
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "mkrnav.h"
//#include "sk_start.h"
//#include "sk_config.h"
#include "md5.h"
#include "sk_net_mon.h"
#include "sk_http_comp.h"
#include "sk_io.h"
#include "websvc/websvc_address.h"
#include "websvc/web_date_format.h"
#include "sk_main.h"
#include "sk_ssl.h"

#if defined(ANDROID) || defined(GTK)
#define __SSL__
#endif


typedef struct sk_net_data_t {
   Sk_NetConnectCallback callback;
   void *context;
   char packet[255];
} Sk_NetData;

struct sk_socket_t {
   int s;
   int is_compressed;
   int is_secured;
   Sk_HttpCompCtx compress_ctx;
   void  *ssl_ctx;
   Sk_IO   *connect_io;
};

//static Sk_ConfigDescriptor Sk_ConfigNetCompressEnabled =
//                        SK_CONFIG_ITEM( "Network", "Compression Enabled");

static int  Sk_NetNumConnects;
static BOOL Sk_NetCompressEnabled = FALSE;

static const char* GetProxyAddress() {
#ifdef IPHONE
   return (sk_main_get_proxy ("http://www.mkr.com"));
#endif //IPHONE
   return NULL;
}

#define CONNECT_TIMEOUT_SEC 5

static void connect_callback (Sk_Socket s, Sk_NetData *data);
static void io_connect_callback (Sk_IO *io);

static int sk_net_send_ssl (Sk_Socket s, const void *data, int length, int wait);
static struct servent* sk_net_getservbyname( const char *name, const char *protocol );
static void *sk_net_connect_internal (const char *protocol, const char *name, const char *resolved_name,
                                                   time_t update_time,
                                                   int default_port,
                                                   int async,
                                                   int flags,
                                                   Sk_NetConnectCallback callback,
                                                   void *context,
                                                   Sk_IO *reuse_connect_io,
                                                   int num_retries);

static void check_connect_timeout (void) {
   Sk_IO *io;

   time_t timeout = time(NULL) - CONNECT_TIMEOUT_SEC;

   while ((io = sk_main_output_timedout(timeout))) {
      Sk_NetData *data =(Sk_NetData *) io->context;
      Sk_Socket s = io->os.socket;
      Sk_IO retry_io = *io;
      Sk_IO *reuse_connect_io;
      char *protocol = strdup(retry_io.retry_params.protocol);
      char *name = strdup(retry_io.retry_params.name);
      char *resolved_name = strdup(retry_io.retry_params.resolved_name);
      
      if (retry_io.retry_params.protocol && retry_io.retry_params.protocol[0]) {
         free(retry_io.retry_params.protocol);
      }
      if (retry_io.retry_params.name && retry_io.retry_params.name[0]) {
         free(retry_io.retry_params.name);
      }
      if (retry_io.retry_params.resolved_name && retry_io.retry_params.resolved_name[0]) {
         free(retry_io.retry_params.resolved_name);
      }

      sk_log(SK_ERROR, "Connect time out (%d)", s->s);
      reuse_connect_io = s->connect_io;
      s->connect_io = NULL;
      sk_main_remove_input(io);
      sk_net_close(s);      

      if (retry_io.retry_params.num_retries < 2) {
         Sk_NetNumConnects--;
         retry_io.retry_params.num_retries++;
         sk_log(SK_ERROR, "Retrying connection (retry # %d)", retry_io.retry_params.num_retries);
         if (SK_INVALID_SOCKET != sk_net_connect_internal (protocol, name, resolved_name,
                                                                     retry_io.retry_params.update_time,
                                                                     retry_io.retry_params.default_port,
                                                                     TRUE,
                                                                     retry_io.retry_params.flags,
                                                                     retry_io.retry_params.callback,
                                                                     retry_io.retry_params.context,
                                                                     reuse_connect_io,
                                                                     retry_io.retry_params.num_retries)) {
            free(protocol);
            free(name);
            free(resolved_name);
            free(data);
            continue;
         }
      }
      
      free(protocol);
      free(name);
      free(resolved_name);
      connect_callback(SK_INVALID_SOCKET, data);
   }
}

static void connect_callback (Sk_Socket s, Sk_NetData *data) {
   Sk_NetConnectCallback callback = data->callback;
   void *context = data->context;

   Sk_NetNumConnects--;

   if (Sk_NetNumConnects == 0) {
      sk_main_remove_periodic(check_connect_timeout);
   }
   
   if ((s != SK_INVALID_SOCKET) && *data->packet) {
      if (!s->is_secured) {
         if( -1 == sk_net_send(s, data->packet,
                                    (int)strlen(data->packet), 1)) {
            sk_log( SK_ERROR, "sk_net callback (HTTP) - Failed to send the 'POST' packet");
            sk_net_close(s);
            s = SK_INVALID_SOCKET;
         }
      } else {
         if( -1 == sk_net_send_ssl(s, data->packet,
                                    (int)strlen(data->packet), 1)) {
            sk_log( SK_ERROR, "sk_net callback (HTTP) - Failed to send the 'POST' packet");
            sk_net_close(s);
            s = SK_INVALID_SOCKET;
         }
      }

      
   }

   free(data);
   
   if (s == SK_INVALID_SOCKET)
       (*callback) (s, context, err_net_failed);
   else
      (*callback) (s, context, succeeded);   
}

static void on_ssl_open_callback (Sk_Socket s, void *data, void *context, sk_result res) {
   s->ssl_ctx = context;
   if (res == succeeded) {
      connect_callback(s, (Sk_NetData *)data);
   } else {
      connect_callback(SK_INVALID_SOCKET, (Sk_NetData *)data);
   }

}

static void io_connect_callback (Sk_IO *io) {

   Sk_NetData *data =(Sk_NetData *) io->context;
   Sk_Socket s;
   LOGI("io_connect_callback_0...");
   s = io->os.socket;
   if (s != SK_INVALID_SOCKET) {
      if (io->retry_params.protocol && io->retry_params.protocol[0]) {
         free(io->retry_params.protocol);
      }
      if (io->retry_params.name && io->retry_params.name[0]) {
         free(io->retry_params.name);
      }

      if (io->retry_params.resolved_name && io->retry_params.resolved_name[0]) {
         free(io->retry_params.resolved_name);
      }
      
      sk_main_remove_input(io);
   }
   LOGI("io_connect_callback_1...");
   if (!s->is_secured) {
      connect_callback(s, data);
   }
#ifdef __SSL__
   else {
      sk_ssl_open(s, data, on_ssl_open_callback);
   }
#endif
}

static Sk_Socket create_socket (const char *protocol,
                                    const char *name,
                                    int default_port,
                                    struct sockaddr *saddr) {
   int   s;
   char *hostname;
   char *separator = strchr (name, ':');

   struct hostent *host = NULL;
   struct sockaddr_in *addr = (struct sockaddr_in *)saddr;

   Sk_Socket sock_struct;

  // sk_net_mon_connect ();

   addr->sin_family = AF_INET;

   hostname = strdup(name);
   sk_check_allocated(hostname);
   //LOGI("HostName:%s\r\n",hostname);
   if (separator != NULL) {

      struct servent *service = NULL;

#ifndef IPHONE
      service = sk_net_getservbyname( separator+1, "tcp" );
#endif

      if (service == NULL) {

         if (isdigit(separator[1])) {

            addr->sin_port = htons(atoi(separator+1));

            if (addr->sin_port == 0) {
               sk_log (SK_ERROR, "invalid port in '%s'", name);
               goto connection_failure;
            }

         } else {
            sk_log (SK_ERROR, "invalid service in '%s'", name);
            goto connection_failure;
         }

      } else {
         addr->sin_port = service->s_port;
      }

      *(strchr(hostname, ':')) = 0;


   } else {
      addr->sin_port = htons(default_port);
   }

   if (!isdigit(hostname[0])) {
      //sk_log(SK_INFO, "Calling gethostbyname_1:%s", hostname);
      host = (struct hostent *)gethostbyname(hostname);
   }

   if (host == NULL) {

      if (isdigit(hostname[0])) {
         addr->sin_addr.s_addr = inet_addr(hostname);
         if (addr->sin_addr.s_addr == INADDR_NONE) {
            sk_log (SK_ERROR, "invalid IP address '%s'", hostname);
            goto connection_failure;
         }
      } else {
         sk_log (SK_ERROR, "invalid host name '%s'", hostname);
         goto connection_failure;
      }

   } else {
      memcpy (&addr->sin_addr, host->h_addr, host->h_length);
   }

   if (strcmp (protocol, "udp") == 0) {
      s = socket (PF_INET, SOCK_DGRAM, 0);
   } else if (strcmp (protocol, "tcp") == 0) {
      s = socket (PF_INET, SOCK_STREAM, 0);
   } else {
      sk_log (SK_ERROR, "unknown protocol %s", protocol);
      goto connection_failure;
   }

   if (s < 0) {
      sk_log (SK_ERROR, "cannot create socket, errno = %d", errno);
      goto connection_failure;
   }

   sock_struct = (Sk_Socket) calloc(sizeof(struct sk_socket_t), 1);
   sock_struct->s = s;

   free(hostname);
   LOGI("Calling gethostbyname:SUCCESS ");
   return sock_struct;

connection_failure:

   free(hostname);
  // sk_net_mon_disconnect();
  // sk_net_mon_error("Error connecting.");
   LOGE( "Calling gethostbyname:ERROR ");
   return SK_INVALID_SOCKET;
}


static int create_connection (Sk_Socket s, struct sockaddr *addr) {

   if (connect (s->s, addr, sizeof(*addr)) < 0) {

      return -1;
   }

   return 0;
}

#ifndef IPHONE
static int create_async_connection (Sk_IO *io, struct sockaddr *addr) {

   int s_flags;
   int res;

   Sk_Socket s = io->os.socket;
   s_flags = fcntl(s->s, F_GETFL, 0);
   if (fcntl(s->s, F_SETFL, s_flags|O_NONBLOCK) == -1) {
      sk_log (SK_ERROR, "Can't set socket nonblocking, errno = %d", errno);
   }
   
   if ((res = connect (s->s, addr, sizeof(*addr))) < 0) {

      if (errno != EINPROGRESS) {
         return -1;
      }

      if (fcntl(s->s, F_SETFL, s_flags) == -1) {
         sk_log (SK_ERROR, "Can't reset socket to blocking, errno = %d", errno);
         return -1;
      }
   }
   LOGI("create_async_connection..sk_main_set_output\r\n");
   sk_main_set_output(io, io_connect_callback, TRUE);
   Sk_NetNumConnects++;

   if (res == 0) {
      /* Probably not realistic */
      io_connect_callback(io);
	  LOGI("create_async_connection..return_0\r\n");
      return 0;
   }

   if (Sk_NetNumConnects == 1) {
      sk_main_set_periodic(CONNECT_TIMEOUT_SEC * 1000 /2, check_connect_timeout);
   }

   LOGI("create_async_connection..return_1\r\n");
   return 0;
}
#endif


static void *sk_net_connect_internal (const char *protocol, const char *name, const char *resolved_name,
                                         time_t update_time,
                                         int default_port,
                                         int async,
                                         int flags,
                                         Sk_NetConnectCallback callback,
                                         void *context,
                                         Sk_IO *reuse_connect_io,
                                         int          num_retries) {

   char            server_url  [ WSA_SERVER_URL_MAXSIZE   + 1];
   char            resolved_server_url  [ WSA_SERVER_URL_MAXSIZE   + 1];
   char            service_name[ WSA_SERVICE_NAME_MAXSIZE + 1];
   int             server_port;
   const char*     proxy_address = GetProxyAddress();
   char            packet[512];
   char            update_since[WDF_MODIFIED_HEADER_SIZE + 1];
   Sk_Socket   res_socket, temp_socket;
   const char *   req_type = "GET";
   struct sockaddr addr;
   Sk_NetData *data = NULL;
   Sk_IO      *io;

   LOGI("sk_net_connect_internal..n=%s p=%s\r\n",name,protocol);

   if( strncmp( protocol, "http", 4) != 0) {
      temp_socket = create_socket(protocol, name, default_port, &addr);

      if(SK_INVALID_SOCKET == temp_socket) return NULL;
      
         res_socket = temp_socket;

      if (async) {
         data = (Sk_NetData *)malloc(sizeof(Sk_NetData));
         data->packet[0] = '\0';
      }
   } else {
      // HTTP Connection, using system configuration for Proxy

      WDF_FormatHttpIfModifiedSince (update_time, update_since);
      if (!strcmp(protocol, "http_post")) req_type = "POST";

      if( !WSA_ExtractParams( name,          //   IN        -   Web service full address (http://...)
                              server_url,    //   OUT,OPT   -   Server URL[:Port]
                              &server_port,  //   OUT,OPT   -   Server Port
                             service_name)) //   OUT,OPT   -   Web service name
      {
         LOGE( "sk_net_connect(HTTP) - Failed to extract information from '%s'", name);
         return SK_INVALID_SOCKET;
      }
      
      if( !WSA_ExtractParams( resolved_name,          //   IN        -   Web service full address (http://...)
                             resolved_server_url,    //   OUT,OPT   -   Server URL[:Port]
                             &server_port,  //   OUT,OPT   -   Server Port
                             service_name)) //   OUT,OPT   -   Web service name
      {
         LOGE(  "sk_net_connect(HTTP) - Failed to extract information from '%s'", resolved_name);
         return SK_INVALID_SOCKET;
      }

      if (proxy_address) {
         int   proxy_port  = server_port;
         char* port_offset = strchr(proxy_address, ':');
         if (port_offset) proxy_port = atoi(port_offset + 1);

         temp_socket = create_socket("tcp", proxy_address, proxy_port, &addr);

         sprintf(packet,
                 "%s %s HTTP/1.0\r\n"
                 "Host: %s\r\n"
                 "User-Agent: FreeMap/%s\r\n"
                 "%s"
                 "%s",
                 req_type, name, server_url, Mkr::sk_start_version(),
                 TEST_NET_COMPRESS( flags ) ? "Accept-Encoding: gzip, deflate\r\n" : "",
                 update_since); 
      } else {
         LOGI("sk_net_connect_internal..Create socket n=%s \r\n",resolved_server_url);
         temp_socket = create_socket("tcp", resolved_server_url, server_port, &addr);

         sprintf(packet,
                 "%s %s HTTP/1.0\r\n"
                 "Host: %s\r\n"
                 "User-Agent: FreeMap/%s\r\n"
                 "%s"
                 "%s",
                 req_type, service_name, server_url, Mkr::sk_start_version(),
                 TEST_NET_COMPRESS( flags ) ? "Accept-Encoding: gzip, deflate\r\n" : "",
                 update_since);
         
      }

      if(SK_INVALID_SOCKET == temp_socket) return SK_INVALID_SOCKET;
      
         res_socket = temp_socket;
      
      res_socket->is_compressed = TEST_NET_COMPRESS( flags );
      res_socket->is_secured = (server_port == 443); //TODO: consider different check

      if (async) {
         data = (Sk_NetData *)malloc(sizeof(Sk_NetData));
         strncpy_safe(data->packet, packet, sizeof(data->packet));
      }
   }
   
   if (!reuse_connect_io)
	io = (Sk_IO *)malloc(sizeof(Sk_IO));
   else
      io = reuse_connect_io;
   
   io->os.socket = res_socket;
   io->os.socket->connect_io = io;
   LOGI("IO Trying to connect..%d sa_data=%s\r\n",async,addr.sa_data);
   if (async) {
      data->callback = callback;
      data->context = context;

      io->subsystem = SK_IO_NET;
      io->context = data;

      io->retry_params.num_retries = num_retries;
      io->retry_params.protocol = strdup(protocol);
      io->retry_params.name = strdup(name);
      io->retry_params.resolved_name = strdup(resolved_name);
      io->retry_params.update_time = update_time;
      io->retry_params.default_port = default_port;
      io->retry_params.flags = flags;
      io->retry_params.callback = callback;
      io->retry_params.context = context;

#ifdef IPHONE
      if (sk_main_async_connect(io, &addr, io_connect_callback) == -1)
#else
      if (create_async_connection(io, &addr) == -1)
#endif
      {
         LOGE("async conn failed..\r\n");
		  free(data);
         sk_net_close(res_socket);
         return NULL;
      }

#ifdef IPHONE
      Sk_NetNumConnects++;

      if (Sk_NetNumConnects == 1) {
         sk_main_set_periodic(CONNECT_TIMEOUT_SEC * 1000 /2, check_connect_timeout);
      }
#endif

   } else {

      /* Blocking connect */
      if (create_connection(res_socket, &addr) == -1) {
         sk_net_close(res_socket);
         return NULL;
      }

      if( strncmp(protocol, "http", 4) == 0) {
         if(-1 == sk_net_send(res_socket, packet, (int)strlen(packet), 1)) {
            sk_log( SK_ERROR, "sk_net_connect(HTTP) - Failed to send the 'POST' packet");
            sk_net_close(res_socket);
            return NULL;
         }
      }
   }

   return io;
}


int sk_net_get_fd(Sk_Socket s) {
   return s->s;
}

Sk_Socket sk_net_connect (const char *protocol, const char *name,
                                   time_t update_time,
                                   int default_port,
                                   int flags,
                                   sk_result* err) {

   Sk_IO *io;
   Sk_Socket s;
   
   if (err != NULL)
   (*err) = succeeded;

   io = (Sk_IO*)sk_net_connect_internal(protocol, name, name, update_time,
                                                   default_port, 0, flags, NULL, NULL, NULL, 0);

   if (io == NULL)
      return NULL;
   
   s = io->os.socket;
   
   if ((s == SK_INVALID_SOCKET) && (err != NULL))
      (*err) = err_net_failed;
   return s;
}


void *sk_net_connect_async (const char *protocol, const char *name, const char *resolved_name,
                               time_t update_time,
                               int default_port,
                               int flags,
                               Sk_NetConnectCallback callback,
                               void *context) {

   return sk_net_connect_internal
                        (protocol, name, resolved_name, update_time, default_port, 1, flags, callback, context, NULL, 0);
}

void sk_net_cancel_connect (Sk_IO *io) {
   Sk_NetData *data = (Sk_NetData *)io->context;
   Sk_Socket s = io->os.socket;
   
   if ( io == NULL || io->subsystem == SK_IO_INVALID )
      return;

   if (io->retry_params.protocol && io->retry_params.protocol[0]) {
      free(io->retry_params.protocol);
   }
   if (io->retry_params.name && io->retry_params.name[0]) {
      free(io->retry_params.name);
   }
   if (io->retry_params.resolved_name && io->retry_params.resolved_name[0]) {
      free(io->retry_params.resolved_name);
   }
   
   sk_log(SK_DEBUG, "Cancelling async connect request (%d)", s->s);
   sk_main_remove_input(io);
   sk_net_close(s);
   free(data);
   
   Sk_NetNumConnects--;
   
   if (Sk_NetNumConnects == 0) {
      sk_main_remove_periodic(check_connect_timeout);
   }
}

int sk_net_send_async( Sk_Socket s, const void *data, int length )
{

   int old_flags;
   int result;

   old_flags = fcntl(s->s, F_GETFL, 0);

   // Set the socket non blocking
   if ( fcntl( s->s, F_SETFL, old_flags|O_NONBLOCK) == -1 )
   {
      sk_log( SK_ERROR, "Can't set socket nonblocking. Error: %d ( %s )", errno, strerror( errno ) );
      result = -1;
   }

   if ( s->is_secured )
   {
      result = sk_net_send_ssl( s, data, length, 0 );
   }
   else
   {  // Not secured
      if ( ( result = send( s->s, data, length, 0 ) ) < 0 )
      {
         if ( errno == EAGAIN || errno == EWOULDBLOCK )
         {
            sk_log( SK_WARNING, "Socket buffer is full. Error: %d ( %s )", errno, strerror( errno ) );
            result = -1;
         }
         else
         {
            sk_log( SK_ERROR, "Error sending data. Error: %d ( %s )", errno, strerror( errno ) );
            result = -1;
         }
      }
   }

   if ( fcntl( s->s, F_SETFL, old_flags ) == -1 ) {
      sk_log (SK_ERROR, "Can't reset socket to blocking, errno = %d", errno);
      result = -1;
   }

   return result;
}


int sk_net_send (Sk_Socket s, const void *data, int length, int wait) {

   int total = length;
   fd_set fds;
   struct timeval recv_timeout = {0, 0};
   
   if (s->is_secured) {
      return sk_net_send_ssl( s, data, length, wait);
   }

   FD_ZERO(&fds);
   FD_SET(s->s, &fds);

   if (wait) {
      recv_timeout.tv_sec = 60;
   }

   while (length > 0) {
      int res;

      res = select(s->s + 1, NULL, &fds, NULL, &recv_timeout);

      if(!res) {
         sk_log (SK_ERROR,
               "Timeout waiting for select in sk_net_send");

         sk_net_mon_error("Error in send - timeout.");

         if (!wait) return 0;
         else return -1;
      }

      if(res < 0) {
         sk_log (SK_ERROR,
               "Error waiting on select in sk_net_send");

         sk_net_mon_error("Error in send - select.");
         return -1;
      }

      res = send(s->s, data, length, 0);

      if (res < 0) {
         sk_log (SK_ERROR, "Error sending data: (%d) %s", errno, strerror(errno));

         sk_net_mon_error("Error in send - data.");
         return -1;
      }

      length -= res;
      data = (char *)data + res;

      sk_net_mon_send(res);
   }

   return total;
}


int sk_net_receive (Sk_Socket s, void *data, int size) {

   int total_received = 0, received;
   void *ctx_buffer;
   int ctx_buffer_size;
   
   if (s->is_compressed) {

      if (!s->compress_ctx) {
         s->compress_ctx = sk_http_comp_init();
         if (s->compress_ctx == NULL) return -1;
      }
      
      sk_http_comp_get_buffer(s->compress_ctx, &ctx_buffer, &ctx_buffer_size);
      
      if (!s->is_secured)
         received = read(s->s, ctx_buffer, ctx_buffer_size);
      else
         received = sk_ssl_read(s->ssl_ctx, ctx_buffer, ctx_buffer_size);
      
      sk_http_comp_add_data(s->compress_ctx, received);
      
      while ((received = sk_http_comp_read(s->compress_ctx, (char*)data + total_received, size - total_received))
             != 0) {
         if (received < 0) {
            sk_net_mon_error("Error in recv.");
            sk_log (SK_DEBUG, "Error in recv. - comp returned %d", received);
            return -1;
         }
         
         total_received += received;
      }
   } else {
      if (!s->is_secured)
         total_received = read (s->s, data, size);
      else
         total_received = sk_ssl_read (s->ssl_ctx, data, size);
   }

   if (total_received < 0) {
      sk_net_mon_error("Error in recv.");
      sk_log (SK_DEBUG, "Error in recv., errno = %d", errno);
      return -1; /* On UNIX, this is sign of an error. */
   }

   sk_net_mon_recv(total_received);

   return total_received;
}
 

static int sk_net_send_ssl (Sk_Socket s, const void *data, int length, int wait) {
#ifndef IPHONE
   return sk_ssl_send ((Sk_SslIO)s->ssl_ctx, data, length);
#endif
}


Sk_Socket sk_net_listen(int port) {

   return SK_INVALID_SOCKET; // Not yet implemented.
}


Sk_Socket sk_net_accept(Sk_Socket server_socket) {

   return SK_INVALID_SOCKET; // Not yet implemented.
}


void sk_net_close (Sk_Socket s) {
   sk_net_mon_disconnect();
   if (s->connect_io)
   free(s->connect_io);
   if (s->is_secured) sk_ssl_close (s->ssl_ctx);
   close (s->s);
   if (s->compress_ctx) sk_http_comp_close(s->compress_ctx);
   free(s);
}


int sk_net_unique_id (unsigned char *buffer, unsigned int size) {
   struct MD5Context context;
   unsigned char digest[16];
   time_t tm;

   time(&tm);

   MD5Init (&context);
   MD5Update (&context, (unsigned char *)&tm, sizeof(tm));
   MD5Final (digest, &context);

   if (size > sizeof(digest)) size = sizeof(digest);
   memcpy(buffer, digest, size);

   return size;
}


static struct servent* sk_net_getservbyname( const char *name, const char *protocol )
{
   struct servent* service = NULL;
   
#ifndef IPHONE
   static int has_bug = -1;   /* Android bug overriding (returns the port in the wrong order */

   if ( has_bug < 0 )
   {
      service = getservbyname( "http", NULL );
        has_bug = ( service == NULL || service->s_port == 80 );
   }

   service = getservbyname( name, protocol );
    if ( service && has_bug )
    {
        service->s_port = htons( service->s_port );
    }
#endif

   return service;
}

void sk_net_set_compress_enabled( BOOL value )
{
   Sk_NetCompressEnabled = value;
}

BOOL sk_net_get_compress_enabled( void )
{
   return Sk_NetCompressEnabled;
}


void sk_net_shutdown (void) {

  /* const char* netcompress_cfg_value = Sk_NetCompressEnabled ? "yes" : "no";
   sk_config_set( &Sk_ConfigNetCompressEnabled, netcompress_cfg_value );*/
   sk_net_mon_destroy();
}

void sk_net_initialize (void) {
   /*sk_config_declare
       ( "user", &Sk_ConfigNetCompressEnabled, "no", NULL );
   Sk_NetCompressEnabled = sk_config_match( &Sk_ConfigNetCompressEnabled, "yes" );*/

   sk_net_mon_start ();
}

int sk_net_socket_secured (Sk_Socket s) {
   return s->is_secured;
}
