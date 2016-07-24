#ifndef MKR_TCPCLIENT_H_
#define MKR_TCPCLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "my_types.h"

#include <mosquittopp.h>

#include "my_time.h"
#include "Dbg.h"
#include "Global.h"
#include <queue>          // std::queue
#include "../network/NetComm.h"
#include "../BizLogic/hos/hos_interface.h"

#include "network/my_socket.h"


enum TcpClientStates
{
	TCP_UNKNOWN,
	TCP_OPEN,
	TCP_CONNECT,
	TCP_RECONNECT,
	TCP_CONNECTED,
	TCP_SEND_ACK,
	TCP_DISCONNECTED
};

class MTcpClient
{

	bool terminate_tcp_client = false;
	bool initialized = false;
	//time_t prev_time = 0, prev_hos_engine_time = 0;
	int64_t prev_time = 0;
	pthread_t thread = { 0 };
	MySocket* socket = 0;
	static MTcpClient *this_instance;
	int state = 0;

	int bytesRead = 0;
	int timeout = 0;


	void init_socket();
	void close_socket();
	void write_req();
	void read_data();
public:
	MTcpClient()
	{

	}
	~MTcpClient()
	{

	}
	static MTcpClient *Get()
	{
		if (this_instance == 0)
			this_instance = new MTcpClient();
		return this_instance;
	}
	void *doIt();

	static void *thread_helper_function(void *context)
	{
		MTcpClient *c = (MTcpClient *)context;
		return c->doIt();
	}

	void  startTcpClient();
	void stopTcpClient();
};

#endif
