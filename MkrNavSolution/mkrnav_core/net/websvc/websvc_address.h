/*
 
 */


#ifndef  __WEBSVC_ADDRESS_H__
#define  __WEBSVC_ADDRESS_H__
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#include "websvc/websvc_address_defs.h"
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"   {
#endif   //   __cplusplue
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL  WSA_IsValid(   const char* szWebServiceAddress);

BOOL  WSA_ExtractParams(   
            const char* szWebServiceAddress, //   IN        -   Web service full address (http://...)
            char*       pServerURL,          //   OUT,OPT   -   Server URL[:Port]
            int*        pServerPort,         //   OUT,OPT   -   Server Port
            char*       pServiceName);       //   OUT,OPT   -   Web service name

void  WSA_RemovePortNumberFromURL(
            char*       szWebServiceAddress);//   IN,OUT    -   Web service full address (http://...)
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef   __cplusplus
};
#endif   // __cplusplue
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
#endif   // __WEBSVC_ADDRESS_H__

