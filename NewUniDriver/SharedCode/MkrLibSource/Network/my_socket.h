#ifndef MY_SOCKET_H_INCLUDED
#define MY_SOCKET_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "Convert.h"
#include "TcpRequest.h"

#include "my_types.h"

//int m_connect(char *hostname, int port);



class MySocket
{
	int sockfd;
public:
	MySocket()
	{
	}
	~MySocket() {}
	int getSocket() { return sockfd; }
	void setTimeout(int to);
	int socket_read(U8 *buffer, int maxlen,bool flag);
	int m_connect(char *hostname, int port/*, int maxTimeout*/);
	int m_connect_ip(char *ip_addr, int port/*, int maxTimeout*/);
	int socket_write(U8 *data, int size);
	void m_close();
};

#endif
