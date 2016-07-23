/*

 */


#ifndef  __WEBSVC_ADDRESS_DEFS_H__
#define  __WEBSVC_ADDRESS_DEFS_H__
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#include "../../core/mkrnav.h"

#define  WSA_PREFIX                       ("http://")
#define  WSA_PREFIX_SIZE                  (sizeof(WSA_PREFIX)-1)
#define  WSA_SSL_PREFIX                   ("https://")
#define  WSA_SSL_PREFIX_SIZE              (sizeof(WSA_SSL_PREFIX)-1)

#define  WSA_SERVER_DEFAULT_PORT          (80)
#define  WSA_SERVER_URL_MAXSIZE           (64)
#define  WSA_SERVER_PORT_DELIMITER        (':')
#define  WSA_SERVER_PORT_MAXSIZE          (10)
#define  WSA_SERVER_PORT_INVALIDVALUE     (0)
#define  WSA_SERVER_URL_DELIMITER         ('/')
#define  WSA_SERVICE_NAME_MAXSIZE         (512)

#define  WSA_STRING_MINSIZE               (  WSA_PREFIX_SIZE               +   \
                                             5   /*   Address name      */ +   \
                                             1   /*   Address delimiter */ +   \
                                             1   /*   Service name      */)
                                  
#define  WSA_STRING_MAXSIZE               (  WSA_PREFIX_SIZE               +   \
                                             WSA_SERVER_URL_MAXSIZE        +   \
                                             1   /*   Port delimiter    */ +   \
                                             WSA_SERVER_PORT_MAXSIZE       +   \
                                             1   /*   Address delimiter */ +   \
                                             WSA_SERVICE_NAME_MAXSIZE)
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#endif   //   __WEBSVC_ADDRESS_DEFS_H__

