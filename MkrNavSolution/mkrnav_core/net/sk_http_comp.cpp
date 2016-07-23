/* sk_http_comp.c - HTTP compression support
  *
 */

#include "stdlib.h"
#include "string.h"
#include "zlib.h"
#include "sk_http_comp.h"

/* states */
#define STATE_READ_HEADERS        1
#define STATE_SEND_HEADERS        2
#define STATE_NO_COMPRESSION_FLOW 3
#define STATE_DEFLATE_READ_HEADER 4
#define STATE_COMPRESSION_FLOW    5
#define STATE_SEND_ACK            6

static const char deflate_magic[2] = {'\037', '\213' };

/* flags */
static const int FLAG_COMPRESSED = 0x1;
static const int FLAG_ERROR      = 0x10;

/* Buffer must be big enough to fit a complete http header */
#define READ_BUFFER_SIZE 1024

struct sk_http_comp_t {
   int state;
   int flags;

   char buffer[READ_BUFFER_SIZE+1];
   int buffer_ptr;
   int read_ptr;
   int read_size;

   z_stream stream;
   unsigned int crc;
};

static const int ZLIB_WIN_SIZE = 15;

/* process http headers and see if we got a compressed response.
 * NOTE: we assume that the whole header can fit our buffer
 */
static void process_headers (Sk_HttpCompCtx ctx) {
   char *ptr;
   if ((ptr = strstr(ctx->buffer, "\r\n\r\n")) != NULL) {

      /* We got all the headers so we can now send to the consumer */
      ctx->state = STATE_SEND_HEADERS;
      ctx->read_size = ptr - ctx->buffer + 4;

      if (strstr(ctx->buffer, "Content-Encoding: gzip") != NULL) {
         ctx->flags |= FLAG_COMPRESSED;
      }
   } else if ((ptr = strstr(ctx->buffer, "ack\r\n")) != NULL) {
      
      /* We got ack so we can now send to the consumer */
      ctx->state = STATE_SEND_ACK;
      ctx->read_size = ptr - ctx->buffer + strlen("ack\r\n");
   }

}

Sk_HttpCompCtx sk_http_comp_init (void) {

   int res;
   Sk_HttpCompCtx ctx = (Sk_HttpCompCtx) calloc(1, sizeof(struct sk_http_comp_t));

   sk_check_allocated(ctx);

   if ((res = inflateInit2( &ctx->stream, -1 * ZLIB_WIN_SIZE )) != Z_OK) {
      sk_log(SK_ERROR, "Error initializing deflate - %d\n", res);
      inflateEnd(&ctx->stream);
      free(ctx);
      return NULL;
   }

   ctx->state = STATE_READ_HEADERS;

   return ctx;
}

void sk_http_comp_get_buffer (Sk_HttpCompCtx ctx,
      void **ctx_buffer, int *ctx_buffer_size) {

   *ctx_buffer = ctx->buffer + ctx->buffer_ptr;
   *ctx_buffer_size = READ_BUFFER_SIZE - ctx->buffer_ptr;
}

/* Called by the network layer when new data is read from the socket */
void sk_http_comp_add_data (Sk_HttpCompCtx ctx, int received) {

   if (received == -1) {
      ctx->flags |= FLAG_ERROR;
      return;
   } else {
      ctx->buffer_ptr += received;
      assert(ctx->buffer_ptr <= READ_BUFFER_SIZE);
      ctx->buffer[ctx->buffer_ptr] = '\0';
   }

   if (ctx->state == STATE_READ_HEADERS) {
      process_headers(ctx);
   }
}

int sk_http_comp_read (Sk_HttpCompCtx ctx, void *data, int size) {

   int res;

   if (ctx->flags & FLAG_ERROR) return -1;

   switch (ctx->state) {

      case STATE_READ_HEADERS:

         /* we need more data */
         return 0;

      case STATE_SEND_HEADERS:
         if (size > (ctx->read_size - ctx->read_ptr)) {
            size = ctx->read_size - ctx->read_ptr;
         }
            
         memcpy(data, ctx->buffer + ctx->read_ptr, size);
         ctx->read_ptr += size;
         if (ctx->read_ptr == ctx->read_size) {
            ctx->read_size = ctx->buffer_ptr;

            if (! (ctx->flags & FLAG_COMPRESSED)) {
               ctx->state = STATE_NO_COMPRESSION_FLOW;
            } else {
               ctx->state = STATE_DEFLATE_READ_HEADER;
            }
         }
         return size;
         
      case STATE_SEND_ACK:
         if (size > (ctx->read_size - ctx->read_ptr)) {
            size = ctx->read_size - ctx->read_ptr;
         }
         
         memcpy(data, ctx->buffer + ctx->read_ptr, size);
         ctx->read_ptr += size;
         if (ctx->read_ptr == ctx->read_size) {
            ctx->read_size = ctx->buffer_ptr;
            
            ctx->state = STATE_READ_HEADERS;
         }
         return size;

      case STATE_NO_COMPRESSION_FLOW:
         if (ctx->buffer_ptr != ctx->read_ptr) {

            if (size > (ctx->buffer_ptr - ctx->read_ptr)) {
               size = ctx->buffer_ptr - ctx->read_ptr;
            }

            memcpy(data, ctx->buffer + ctx->read_ptr, size);
            ctx->read_ptr += size;
            if (ctx->read_ptr == ctx->buffer_ptr) {
               ctx->read_ptr = ctx->buffer_ptr = 0;
            }
            return size;

         } else {
            /* need more data */
            return 0;
         }

      case STATE_DEFLATE_READ_HEADER:
         if ((ctx->buffer_ptr - ctx->read_ptr) < 10) {
            /* Gzip Header size is 10 bytes. */
            return 0;
         }

         /* Check Gzip header. We don't support also ext. header. */
         if ((memcmp(ctx->buffer + ctx->read_ptr, deflate_magic, 2) != 0) ||
              (ctx->buffer[ctx->read_ptr + 3] != 0 )) {

            sk_log(SK_ERROR, "Unsupported gzip header.");
            ctx->flags |= FLAG_ERROR;
            return -1;
         }
            
         /* Ok, eat the header! */
         ctx->read_ptr += 10;
         ctx->state = STATE_COMPRESSION_FLOW;

         /* fall through to STATE_COMPRESSION_FLOW */

      case STATE_COMPRESSION_FLOW:

         if (ctx->read_ptr == ctx->buffer_ptr) {
            /* need more data */
            ctx->read_ptr = ctx->buffer_ptr = 0;
            return 0;
         }

         /* Decompress received data */
         ctx->stream.next_in   = (unsigned char *) (ctx->buffer + ctx->read_ptr);
         ctx->stream.avail_in  = ctx->buffer_ptr - ctx->read_ptr;
         ctx->stream.next_out  = (Bytef*)data;
         ctx->stream.avail_out = size;

         res = inflate( &ctx->stream, Z_SYNC_FLUSH );

         if ((res == Z_STREAM_END) || (res == Z_OK)) {
            /* We are not verifying CRC, ignoring data leftovers, etc.
             * Being lazy saves power :)
             */
            ctx->read_ptr = ctx->buffer_ptr - ctx->stream.avail_in;
            size = size - ctx->stream.avail_out;
         } else {
            sk_log(SK_ERROR, "Error in inflate - %d", res);
            ctx->flags |= FLAG_ERROR;
            return -1;
         }

         /* Ignore compressed stream trailer */
         if (res == Z_STREAM_END) {
            ctx->read_ptr = ctx->buffer_ptr;
         }

         return size;
   }

   return -1;
}

int sk_http_comp_avail (Sk_HttpCompCtx ctx) {
   return ctx->stream.avail_out;
}

void sk_http_comp_close (Sk_HttpCompCtx ctx) {
	if (ctx) free(ctx);
}

