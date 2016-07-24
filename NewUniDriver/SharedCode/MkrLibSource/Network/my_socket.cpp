#include "my_socket.h"
#include "Dbg.h"
#include <errno.h>
#include<arpa/inet.h> //inet_addr

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable : 4127 4389 4018)
#endif

#ifndef AI_NUMERICSERV  // (missing in older Mac SDKs)
#define AI_NUMERICSERV 0x1000
#endif

#if JUCE_WINDOWS
typedef int       juce_socklen_t;
typedef SOCKET    SocketHandle;
#else
typedef socklen_t juce_socklen_t;
typedef int       SocketHandle;
#endif

#define TCP_NODELAY SO_RCVTIMEO


//--------------------------------MySocket-------------
void MySocket::setTimeout(int to)
{
	//set timeout
	struct timeval tv;
	tv.tv_sec = to;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
}
/* blocking and non-blocking can be specified. It has the option, MSG_WAITALL, where it will block until all bytes arrived.*/
//http://stackoverflow.com/questions/12773509/read-is-not-blocking-in-socket-programming
int MySocket::socket_read(U8 *buffer, int maxlen,bool flag)
{
	//LOGI("sock read %x", this->sockfd);
	/*struct sockaddr_in client;
	socklen_t clientLen = sizeof(struct sockaddr_in);
	int n = recvfrom(this->sockfd, buffer, maxlen, 0, (struct sockaddr *)&client, &clientLen);*/
	int n = 0;
	if (!flag)
	{
		n=recv(this->sockfd, buffer, maxlen, 0);
	}
	else {
		n = recv(this->sockfd, buffer, maxlen, MSG_WAITALL);
	}
	if (n < 0)
	{
		//LOGE("socket_read ERROR: %s\n", strerror(errno));
	}
	return n;
}
int MySocket::m_connect(char *hostname, int port/*, int maxTimeout*/)
{
	int rv = 1;
	int  portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	U8 buffer[256];

	portno = port;//oi(argv[2]);

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		rv = -1;
		goto _exit_;
	}
		

	server = gethostbyname(hostname);

	if (server == NULL) {
		
		rv = -2;
		goto _exit_;
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr,

		(char *)&serv_addr.sin_addr.s_addr,

		server->h_length);

	serv_addr.sin_port = htons(portno);



	if (connect(sockfd , (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		//fprintf(stderr, "ERROR connecting addr=%s\n", (char *)server->h_addr);
		rv = -3;
		goto _exit_;
	}
	
	return rv;
_exit_:
	close(sockfd);

	return rv;

}
int MySocket::m_connect_ip(char *ip_addr, int port/*, int maxTimeout*/)
{
	int rv = 1;
	struct sockaddr_in server;
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		rv = -1;
		goto _exit_;
	}
	server.sin_addr.s_addr = inet_addr(ip_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		//fprintf(stderr, "ERROR connecting addr=%s\n", (char *)server->h_addr);
		rv = -3;
		goto _exit_;
	}

	return rv;

_exit_:
	close(sockfd);

	return rv;
}
int MySocket::socket_write(U8 *data, int size)
{
	//LOGI("sock write %x", this->sockfd);
	int n = send(this->sockfd, data, size, 0);
	return n;
}
void MySocket::m_close()
{
	close(this->sockfd);
}
