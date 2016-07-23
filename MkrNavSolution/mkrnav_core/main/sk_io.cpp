/* sk_io.c - a module to hide OS-specific IO operations.
 */

#include "sk_io.h"


int sk_io_read  (Sk_IO *io, void *data, int size) {

   switch (io->subsystem) {

      case SK_IO_FILE:
         return sk_file_read (io->os.file, data, size);

      case SK_IO_NET:
         return sk_net_receive (io->os.socket, data, size);

     /* case SK_IO_SERIAL:
         return sk_serial_read (io->os.serial, data, size);

      case SK_IO_PIPE:
         return sk_spawn_read_pipe (io->os.pipe, data, size);*/

      case SK_IO_NULL:
         return 0; /* Cannot receive anything from there. */
   }
   return -1;
}


int sk_io_write (Sk_IO *io, const void *data, int length, int wait) {

   switch (io->subsystem) {

      case SK_IO_FILE:
         return sk_file_write (io->os.file, data, length);

      case SK_IO_NET:
         return sk_net_send (io->os.socket, data, length, wait);

     /* case SK_IO_SERIAL:
         return sk_serial_write (io->os.serial, data, length);

      case SK_IO_PIPE:
         return sk_spawn_write_pipe (io->os.pipe, data, length);
*/
      case SK_IO_NULL:
         return length; /* It's all done, since there is nothing to do. */
   }
   return -1;
}

int sk_io_write_async (Sk_IO *io, const void *data, int length) {
   
   switch (io->subsystem) {
      case SK_IO_NET:
#if defined(IPHONE_NATIVE) || defined(GTK) || defined(ANDROID)
         return sk_net_send_async( io->os.socket, data, length );
#else
         return sk_net_send (io->os.socket, data, length, 0); //Change this to async once implemented
#endif
      
      case SK_IO_FILE:
      case SK_IO_SERIAL:
      case SK_IO_PIPE:
         return -1;
         
      case SK_IO_NULL:
         return length; /* It's all done, since there is nothing to do. */
   }
   return -1;
}


void  sk_io_close (Sk_IO *io) {

   switch (io->subsystem) {

      case SK_IO_FILE:
         sk_file_close (io->os.file);
         break;

      case SK_IO_NET:
         sk_net_close (io->os.socket);
         break;

      /*case SK_IO_SERIAL:
         sk_serial_close (io->os.serial);
         break;

      case SK_IO_PIPE:
         sk_spawn_close_pipe (io->os.pipe);
         break;*/

      case SK_IO_NULL:
         break;
   }
   sk_io_invalidate( io );
}

void  sk_io_invalidate (Sk_IO *io) {

   switch (io->subsystem) {

      case SK_IO_FILE:
         io->os.file = SK_INVALID_FILE;
         break;

      case SK_IO_NET:
         io->os.socket = SK_INVALID_SOCKET;
         break;

      /*case SK_IO_SERIAL:
         io->os.serial = SK_INVALID_SERIAL;
         break;

      case SK_IO_PIPE:
         io->os.pipe = SK_SPAWN_INVALID_PIPE;
         break;*/

      case SK_IO_NULL:
         break;
   }
   io->subsystem = SK_IO_INVALID;
}


int sk_io_same (Sk_IO *io1, Sk_IO *io2) {

   /* We do not do a memcmp() here because some compilers might
    * generate holes that are not initialized in Sk_IO.
    */
   if (io1->subsystem != io2->subsystem) return 0;

   switch (io1->subsystem) {

      case SK_IO_FILE:
         if (io1->os.file != io2->os.file) return 0;
         break;

      case SK_IO_NET:
         if (io1->os.socket != io2->os.socket) return 0;
         break;

      /*case SK_IO_SERIAL:
         if (io1->os.serial != io2->os.serial) return 0;
         break;

      case SK_IO_PIPE:
         if (io1->os.pipe != io2->os.pipe) return 0;
         break;*/

      case SK_IO_NULL:
         break; /* No reason to be any different from each other. */
   }

   return 1; /* We did not find any difference. */
}

