/* sk_httpcopy_async.h - Download Mkr maps using the HTTP protocol asynchornously.
 
 */

#ifndef INCLUDED__SK_HTTPCOPY__H
#define INCLUDED__SK_HTTPCOPY__H

#include <time.h>

#define HTTPCOPY_FLAG_NONE                               0x00000000
#define HTTPCOPY_FLAG_IGNORE_CONTENT_LEN                 0x00000001


typedef int  (*Sk_HttpAsyncCallbackSize)     (void *context, size_t size);
typedef void (*Sk_HttpAsyncCallbackProgress) (void *context, char *data, size_t size);
typedef void (*Sk_HttpAsyncCallbackError)    (void *context, int connection_failure, const char *format, ...);
typedef void (*Sk_HttpAsyncCallbackDone)     (void *context, char *last_modified, const char *format, ... );

typedef struct {
   Sk_HttpAsyncCallbackSize     size;
   Sk_HttpAsyncCallbackProgress progress;
   Sk_HttpAsyncCallbackError    error;
   Sk_HttpAsyncCallbackDone		done;
} Sk_HttpAsyncCallbacks;

struct HttpAsyncContext_st;
typedef struct HttpAsyncContext_st HttpAsyncContext;

HttpAsyncContext *sk_http_async_copy (Sk_HttpAsyncCallbacks *callbacks,
									  void *context,
                             const char *source,
                             time_t update_time);

void	sk_http_async_copy_abort (HttpAsyncContext *context);

HttpAsyncContext * sk_http_async_post( Sk_HttpAsyncCallbacks *callbacks, void *context,
                                                 const char *source, const char* header, const void* data, int data_length, int flags );

const char* sk_http_async_get_simple_header( const char* content_type, int content_len );

HttpAsyncContext * sk_http_async_post_file( Sk_HttpAsyncCallbacks *callbacks, void *context,
                                                const char *source, const char* header, const char *full_name, int size );
const char* sk_http_async_get_upload_header( const char* content_type, const char *full_name, int size,
                                                 const char *user, const char *pw);

#endif // INCLUDED__SK_HTTPCOPY__H
