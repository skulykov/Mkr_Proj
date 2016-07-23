/* sk_http_comp.h
 *
 */

#ifndef  __SK__HTTP_COMP_H__
#define  __SK__HTTP_COMP_H__

#include "mkrnav.h"

struct sk_http_comp_t;
typedef struct sk_http_comp_t *Sk_HttpCompCtx;

Sk_HttpCompCtx sk_http_comp_init (void);
void sk_http_comp_add_data (Sk_HttpCompCtx ctx, int received);
int  sk_http_comp_read (Sk_HttpCompCtx ctx, void *data, int size);
void sk_http_comp_close(Sk_HttpCompCtx ctx);

int sk_http_comp_avail (Sk_HttpCompCtx ctx);
void sk_http_comp_get_buffer (Sk_HttpCompCtx ctx,
      void **ctx_buffer, int *ctx_buffer_size);
#endif   // __SK__HTTP_COMP_H__
