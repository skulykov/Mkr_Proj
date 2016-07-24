#include "tcp_client.h"
#include "poll.h"
#include "errno.h"
#define M_LOG sk_log_to_file
MTcpClient *MTcpClient::this_instance = 0;

static int localPortNumber = 333;
static char * localHostName = "192.168.4.1";

void MTcpClient::init_socket()
{
	if (socket == 0)
	{
		socket = new MySocket();
		//bool binding = socket->bindToPort(localPortNumber);
		//bool listener = socket->createListener(localPortNumber, localHostName);
		socket->setTimeout(10);
	}
}
void MTcpClient::close_socket()
{
	if (socket != 0)
	{
		socket->m_close();
		delete socket;
		socket = 0;
	}
}
void  MTcpClient::startTcpClient()
{
	if (pthread_create(&thread, NULL, MTcpClient::thread_helper_function, this) != 0)
	{
		M_LOG("ERROR TcpClient threadProc... \n");
	}
	initialized = true;
}
void MTcpClient::write_req()
{
	int rv = socket->socket_write((unsigned char*)"REQ\r\n", 5);
	if (rv < 0)
	{
		M_LOG("TcpClient Write Fail rv:%d  \n", rv);
		//disconnected
		state = TcpClientStates::TCP_DISCONNECTED;
	}
	else {
		M_LOG("TcpClient Wrote ack rv:%d  \n", rv);
	}
}
void MTcpClient::read_data()
{
#if 0
	unsigned char buff[256];
	M_LOG("TcpClient Receiving  bytes \n");
	bytesRead = socket->socket_read(buff, 256, false); // blocking
	M_LOG("TcpClient Rec:%d bytes \n", bytesRead);
	if (bytesRead > 0)
	{
		//write_ack();
	}
	else
	{
		if (bytesRead == 0)
		{
			// the peer might be disconnected  or there is nothing to read
			// to make sure just write to it
			//write_ack();        //  fails -> status disconnected
		}
		else if (bytesRead < 0) {
			int errsv = errno;
			M_LOG("TcpClient Read errno :%d  \n", errsv);  //110 - connection timeout
			//write_ack();
		}
	}
#else

	struct pollfd pfd;
	int ret;

	pfd.fd = socket->getSocket(); // your socket handler 
	pfd.events = POLLIN | POLLHUP | POLLRDNORM;
	pfd.revents = 0;
	ret = poll(&pfd, 1, 1000 * 2); // 2 second for timeout
	switch (ret) {
	case -1:
		M_LOG("Tcp Poll Error  \n");// Error
		break;
	case 0:
		//M_LOG("Tcp Poll Timeout  \n");// Timeout 
		timeout++;
		if (timeout > 50)
		{
			//M_LOG("Tcp Poll timeout > 100  \n");
			//state = TcpClientStates::TCP_SEND_ACK;//TcpClientStates::TCP_DISCONNECTED;
			timeout = 0;
		}
		break;
	default:
	{//recv(mySocket, buf, sizeof(buf), 0); // get your data
	 //-------------------------------------------------------------------------
	 // if result > 0, this means that there is either data available on the
	 // socket, or the socket has been closed
		char buffer[256];
		ret = recv(pfd.fd, buffer, sizeof(buffer),/* MSG_PEEK | MSG_DONTWAIT*/0);
		if (ret == 0) {
			// if recv returns zero, that means the connection has been closed:
			state = TcpClientStates::TCP_DISCONNECTED;
			M_LOG("Tcp if recv returns zero, that means the connection has been closed: \n");
		}
		else if (ret > 0)
			//------------------------------------------------------------------------
		{
			//if (pfd.revents & POLLIN)
			//read_data();
			M_LOG("Tcp Read after poll bytesRead=%d  \n", ret);
			//state = TcpClientStates::TCP_SEND_ACK;
			timeout = 0;
			//write_ack();
		}
	}
	}
#endif
}
int main_route();
int main_get_route_info(/*int argc, char *argv[]*/);
void *MTcpClient::doIt()
{
	//main_route();
	main_get_route_info(/*int argc, char *argv[]*/);
	
	state = TcpClientStates::TCP_OPEN;
	while (!terminate_tcp_client) {
		switch (state)
		{
			case TcpClientStates::TCP_OPEN:
			{
				init_socket();
				state = TcpClientStates::TCP_CONNECT;
			}
			break;
			case TcpClientStates::TCP_CONNECT:
			{

				M_LOG("TcpClient trying to connect... \n");
				int rv = socket->m_connect_ip(localHostName, localPortNumber/*, 20000*/);  //connect is a blocking call by default,
				if (rv == 1)
				{
					M_LOG("TcpClient connected... \n");
					state = TcpClientStates::TCP_CONNECTED;
					timeout = 0;
				}
				else {
					M_LOG("TcpClient Failed to connect... \n");
					usleep(4 * 1000 * 1000);                     // sleep 4 sec
				}
			}
			break;
			/*In TCP there is only one way to detect an orderly disconnect, and that is by getting zero as a return value from read()/recv()/recvXXX() when reading.*/
			//http://stackoverflow.com/questions/283375/detecting-tcp-client-disconnect
			case  TcpClientStates::TCP_CONNECTED:
			{
				/* I had an endless blocking read when connection is broken
				To avoid this situation I try to use poll
				*/
				int64_t t = DateTime::Now().getMillis();
				if ((t - prev_time) > 10 * 60 * 1000)
				{
					prev_time = t;
					state = TcpClientStates::TCP_SEND_ACK;
					
				}
				else {
					read_data();
				}
				break;
				

			}
			break;
			case TcpClientStates::TCP_SEND_ACK:
			{
				write_req();
				state = TcpClientStates::TCP_CONNECTED;
			}
			break;
			case TcpClientStates::TCP_DISCONNECTED:
			{
				M_LOG("TcpClientStates::TCP_DISCONNECTED  \n");
				usleep(2 * 1000 * 1000);                     // sleep 4 sec
				close_socket();
				M_LOG("Going to TcpClientStates::TCP_OPEN  \n");
				state = TcpClientStates::TCP_OPEN;
			}
			break;
		}//switch (state)

		usleep(1000 * 100);
	}//while (!terminate_tcp_client)
	initialized = false;
}

void MTcpClient::stopTcpClient()
{
	terminate_tcp_client = true;
}