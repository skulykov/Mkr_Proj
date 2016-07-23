/* sk_file.h - a module to open/read/close a mkr database file.
 *
 
 * DESCRIPTION:
 *
 *   This module provides a unique interface to all types of IO
 *   supported by MkrNav. It is not assumed that the OS provides
 *   a unified API for all sort of IO, thus this module that makes
 *   sure the rest of MkrNav code does not care much about these
 *   details.
 *   The IO must still be open in a IO specific way.
 */

#ifndef INCLUDE__SK_IO__H
#define INCLUDE__SK_IO__H

#include <stdio.h>

#include "filemng.h"
#include "../net/sk_net.h"
//#include "sk_spawn.h"
//#include "sk_serial.h"

/* The list of supported subsystems: */
#define SK_IO_INVALID 0
#define SK_IO_FILE    1
#define SK_IO_NET     2
#define SK_IO_SERIAL  3
#define SK_IO_PIPE    4
#define SK_IO_NULL    5 /* Bottomless pitt (i.e., no IO). */

typedef struct {

   int subsystem;
   void *context;

   union {
      SK_FILE       file;
      Sk_Socket     socket;
      /*Sk_Serial     serial;
      Sk_Pipe       pipe;*/
   } os;

   struct {
      int                        num_retries;
      char                       *protocol;
      char                       *name;
      char                       *resolved_name;
      time_t                     update_time;
      int                        default_port;
      int                        flags;
      Sk_NetConnectCallback  callback;
      void                       *context;
   } retry_params;

} Sk_IO;


int   sk_io_read  (Sk_IO *io, void *data, int size);
int   sk_io_write (Sk_IO *io, const void *data, int length, int wait);
int   sk_io_write_async (Sk_IO *io, const void *data, int length);
void  sk_io_close (Sk_IO *io);
void  sk_io_invalidate (Sk_IO *io);
int sk_io_same (Sk_IO *io1, Sk_IO *io2);

#endif // INCLUDE__SK_IO__H

