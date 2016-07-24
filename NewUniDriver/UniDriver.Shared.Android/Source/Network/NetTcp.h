#ifndef MKR_NETTCP_H_INCLUDED
#define MKR_NETTCP_H_INCLUDED


//#include "juce_Socket.h"
#include "network/TcpRequest.h"
#include "CommonUtils/Dbg.h"



class NetTcp
{
public:
	static int SendAndReceiveData(TcpRequestObj *obj);
	static void LoginRequest(string user, string Password, int rID, request_cb_t cb);
	static void RegisterRequest(string payload, request_cb_t cb);
};



#endif