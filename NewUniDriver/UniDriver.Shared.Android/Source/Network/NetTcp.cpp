#include "NetTcp.h"

#include "network/my_socket.h"

#if 0
//int NetTcp::SendAndReceiveData(TcpRequestObj *obj)
//{
//	int rv1 = -100;
//#if 1
//	int localPortNumber = 2411;
//	char * localHostName ="23.95.120.21";
//	MySocket* socket = new MySocket();
//	//bool binding = socket->bindToPort(localPortNumber);
//	//bool listener = socket->createListener(localPortNumber, localHostName);
//	socket->setTimeout(30);
//	int rv = socket->m_connect(localHostName, localPortNumber, 20000);
//	if (rv==1)
//	{
//		//m_printf("Connected ..OK :" );
//		obj->hdr->serialize();
//		int size = obj->hdr->Size();
//		byte *msg = obj->hdr->GetBuffer(); //{ 0x10, 0, 0, 0, 0x01, 0x22, 0x33, 0x44, 0x55 };
//		//printDump(msg, size);
//		int len=socket->socket_write(msg, size);  // first 4 bytes = total length
//		//m_printf("Connected ..wrote :%d",len);
//		//read response
//		unsigned char buff[2048];
//		int bytesRead = socket->socket_read(buff, 4, true);
//		//m_printf("Connected ..bytesRead :%d", bytesRead);
//		int totalLength = Convert::ToInt32(buff, 0);
//		//m_printf("Connected ..totalLength:%d", totalLength);
//		unsigned char *inBuffer = new unsigned char[totalLength];
//		bytesRead = 0;
//	//	usleep(200000);
//		do
//		{
//			try
//			{
//				int bytes = socket->socket_read(buff, 2048, true);
//				//m_printf("socket->socket_read = %d ..",bytes);
//				if (bytes == -1)
//				{
//					//m_printf("Fatal ERROR bytes=-1 ..!!");
//					delete inBuffer;
//					rv1= -1;
//					goto __exit__;
//				}
//				if (bytes == 0)
//				{
//					//m_printf("Peer dropped conn bytes=0 ..!!");
//					delete inBuffer;
//					rv1= 0;
//					goto __exit__;
//				}
//				memcpy(inBuffer + bytesRead, buff, bytes);//ms.Write(resp, 0, bytes);
//				//TcpLLog.log(" >> " + "Wrote- bytes=" + bytes.ToString());
//				bytesRead += bytes;
//			}
//			catch (/*IOException*/ /*std::bad_alloc &e*/const std::exception& e)
//			{
//				break;
//			}
//		} while (bytesRead < totalLength);
//		/*
//		if (len > 0)
//		{
//		int index = 0;
//		int totalLength = ConvertToInt32(buff,index);
//		char buffer[32];
//		itoa(len, buffer, 10);
//		string result(buffer);
//		//label->setText(result, NotificationType::dontSendNotification);
//		}*/
//
//		obj->SetResponse(inBuffer, bytesRead);
//
//		obj->ExecuteCB();
//
//		delete inBuffer;
//		rv1 = 1;
//	}
//	__exit__:
//	socket->m_close();
//	delete socket;
//#endif
//	return rv1;
//	
//
//}
//
//int main_proto_read();
//void NetTcp::LoginRequest(string user, string Password, int rID, request_cb_t cb)
//{
//
//	ClsLoginMD *lgn = new ClsLoginMD(user, Password, rID);
//    
//	string json=lgn->toJsonString(lgn);
//	TcpRequestObj obj(cb, HttpRequestNumber::Login, json);
//	obj.parObj = lgn;
//	SendAndReceiveData(&obj);
//
//}
#else
int NetTcp::SendAndReceiveData(TcpRequestObj *obj)
{
	int rv1 = -100;
#if 1
	int localPortNumber = 2411;
	char * localHostName = "23.95.120.21";
	MySocket* socket = new MySocket();
	//bool binding = socket->bindToPort(localPortNumber);
	//bool listener = socket->createListener(localPortNumber, localHostName);
	socket->setTimeout(30);
	int rv = socket->m_connect(localHostName, localPortNumber/*, 20000*/);
	if (rv == 1)
	{
		//m_printf("Connected ..OK :" );
		//obj->hdr->serialize();
		//int size = obj->hdr->Size();
		//byte *msg = obj->hdr->GetBuffer(); //{ 0x10, 0, 0, 0, 0x01, 0x22, 0x33, 0x44, 0x55 };
		//   //printDump(msg, size);
		string &request=obj->GetRequest();
		int size = request.length();
		byte buf[4];
		Convert::Int32ToBytes(size, buf, 0);
		socket->socket_write(buf, 4);  // first 4 bytes = total length
		//m_printf("Connected ..OK size:%d",size);
		//printDump((byte*)request.c_str(), size);
		int len = socket->socket_write((byte*)request.c_str(), size);  // first 4 bytes = total length
													//m_printf("Connected ..wrote :%d",len);
													//read response
		unsigned char buff[2048];
		int bytesRead = socket->socket_read(buff, 4, true);
		//m_printf("Connected ..bytesRead :%d", bytesRead);
		int totalLength = Convert::ToInt32(buff, 0);
		//m_printf("Connected ..totalLength:%d", totalLength);
		unsigned char *inBuffer = new unsigned char[totalLength];
		bytesRead = 0;
		//	usleep(200000);
		do
		{
			try
			{
				int bytes = socket->socket_read(buff, 2048, true);
				//m_printf("socket->socket_read = %d ..",bytes);
				if (bytes == -1)
				{
					//m_printf("Fatal ERROR bytes=-1 ..!!");
					delete inBuffer;
					rv1 = -1;
					goto __exit__;
				}
				if (bytes == 0)
				{
					//m_printf("Peer dropped conn bytes=0 ..!!");
					delete inBuffer;
					rv1 = 0;
					goto __exit__;
				}
				memcpy(inBuffer + bytesRead, buff, bytes);//ms.Write(resp, 0, bytes);
														  //TcpLLog.log(" >> " + "Wrote- bytes=" + bytes.ToString());
				bytesRead += bytes;
			}
			catch (/*IOException*/ /*std::bad_alloc &e*/const std::exception& e)
			{
				break;
			}
		} while (bytesRead < totalLength);
		/*
		if (len > 0)
		{
		int index = 0;
		int totalLength = ConvertToInt32(buff,index);
		char buffer[32];
		itoa(len, buffer, 10);
		string result(buffer);
		//label->setText(result, NotificationType::dontSendNotification);
		}*/

		obj->SetResponse(inBuffer, bytesRead);

		obj->ExecuteCB();

		delete inBuffer;
		rv1 = 1;
	}
__exit__:
	socket->m_close();
	delete socket;
#endif
	return rv1;


}

int main_proto_read();
void NetTcp::LoginRequest(string user, string pass, int rID, request_cb_t cb)
{

	ClsLoginMD lgn;
	lgn.set_res(false);
	lgn.set_cid(0);
	lgn.set_user(user);
	lgn.set_pass(pass);
	lgn.set_resourceid(-1);
	lgn.set_drname("");

	string payload=lgn.SerializeAsString();

	TcpRequestObj obj(cb, HttpRequestNumber::Login, payload);
	obj.parObj = 0;
	SendAndReceiveData(&obj);

}
void NetTcp::RegisterRequest(string payload, request_cb_t cb)
{
	TcpRequestObj obj(cb, HttpRequestNumber::RegisterMD, payload);
	obj.parObj = 0;
	SendAndReceiveData(&obj);
}


#endif
