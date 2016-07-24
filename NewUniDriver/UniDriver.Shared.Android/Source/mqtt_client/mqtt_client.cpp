
#include <cstdio>
#include <cstring>
#include "../network/NetComm.h"

#include "my_utils.h"
#include "ClsMqtt.h"
#include "WorkOrderDB.h"

using namespace std;
//const string server_topic = "MI/";
const string prefix = "Bsm/Kiewit/";
const string lgn_topic = prefix + "Login/";

#ifdef WIN32
// fake implementation
void JniApi_PostReceivedMsgProto(int req, int par, void *payload, int payload_len) {}
#endif

string getPrefix()
{
	return prefix;
}

mqtt_client::mqtt_client()
{
}

mqtt_client::~mqtt_client()
{
}

mqtt_client::mqtt_client(const char *id) : mosquittopp(id)
{
}
int mqtt_client::mqtt_connect(const char *host, int port)
{

	int keepalive = 60;

	/* Connect immediately. This could also be done by calling
	* mqtt_client->connect(). */
	LOGI("mqtt_client.. constructor connect %d\n", keepalive);
	connect(host, port, keepalive);
	return 0;
}
//void subscribe_to_messenger()
//{
//	string dn_topic = prefix + Global::Get()->GetDriverID();
//	int mid = 1, mid1 = 2;
//	string topic = lgn_topic + "#";    // subscribe to get all login messages from Login subdirs  
//	subscribe(&mid, topic.c_str());
//	subscribe(&mid1, dn_topic.c_str());
//}
//void subscribe_to_server()
//{
//	
//	int mid = 1;// , mid1 = 2;
//	string topic = server_topic + Global::Get()->GetDriverID();    // subscribe to get responses from server  
//	subscribe(&mid, topic.c_str());
//	//subscribe(&mid1, dn_topic.c_str());
//}
void mqtt_client::on_connect(int rc)
{
	owner->OnConnect(rc);
}
void mqtt_client::on_disconnect(int rc)
{
	owner->OnDisconnect(rc);
	//LOGI("On DISCONNECT,,,\r\n");
}
#if 0
void mqtt_client::on_message(const struct mosquitto_message *message)
{
	string topic = string(message->topic);
	string cur_did = Global::Get()->GetDriverID();
	
	LOGI("Message Received %s %s\n", message->topic, message->payload);//LOGI("%s\n", str);
	return;
	string msg = string((char*)message->payload, message->payloadlen);
	

	if (!msg.empty())
	{
		vector<string> v = split(msg, '|');
		if (v.size() >= 2)
		{
			string did = v[0];
			string msg_body = v[1];
			if (!did.empty() && !msg_body.empty())
			{
				size_t found1 = topic.find(lgn_topic);
						if (found1 != string::npos)
						{
							if (did != cur_did)
							{
								LOGI("user login to this chat room..");
								JniApi_PostAddNewUser(did.c_str(), msg_body.c_str());
							}
						}
						else
						{
							size_t found = topic.find(cur_did);
							if (found != string::npos)
							{
								JniApi_PostReceivedMsg(did.c_str(), msg_body.c_str());
							}
						}
			}
					
		}

	}
		
}
#endif
void mqtt_client::process_message(unsigned char *payload, int payloadlen)
{
	CommPacket commP;
	commP.ParseFromArray(payload, payloadlen);
	LOGI("Message Received req=%d did=%d\n", commP.req(), commP.rid());
	switch (commP.req())
	{
	case (int)HttpRequestNumber::HBReq:
	{
		PMobileHB mhb;
		string pld = commP.payload();
		mhb.ParseFromArray(pld.c_str(), pld.size());
		LOGI("Message Received MHB spd=%d cog=%d\n", mhb.spd(), mhb.cog());
	}
	break;
	case (int)HttpRequestNumber::GetActivities:
	{
		PWorkOrderList wL;
		string pld = commP.payload();
		wL.ParseFromArray(pld.c_str(), pld.size());
		WorkOrderDB::Get()->SaveWorkOrders(&wL);
		int len = wL.wo_items_size();
		LOGI("Message GetActivities len=%d \n", len);
		JniApi_PostReceivedMsgProto((int)HttpRequestNumber::GetActivities, len, payload, payloadlen);
	}
	break;
	}

}
void mqtt_client::process_chat_message(unsigned char *payload, int payloadlen, bool login_flag)
{
	string json((char*)payload, payloadlen);
	JsonMessageCls m;
	vector<JsonMessageCls*> v;
	m.deserializeList(json, v);
	if (v.size() > 0)
	{
		if (login_flag)
		{
			JniApi_PostReceivedMsgProto(1001, 1, payload, payloadlen);
		}
		else {
			// regular message
			JniApi_PostReceivedMsgProto(1002, 2, payload, payloadlen);
		}
	}
}
void mqtt_client::on_message(const struct mosquitto_message *message)
{
	string topic = string(message->topic);
	string cur_did = Global::Get()->GetDriverID();

	LOGI("Message Received Topic=%s \n", message->topic);//LOGI("%s\n", str);
	vector<string> v = split(topic, '/');
	string s = v[0];
	if (v.size() >= 3)
	{
		string s1 = v[1],s2=v[2];
		if (s == "MSG" && s1 == "LOGIN")
		{
			LOGI("Login message for %s\n", s2.c_str());
			process_chat_message((unsigned char*)message->payload, message->payloadlen,true);
		}
		
	}
	else if (v.size() >= 2 && !s.empty() && s== "MI")
	{
		// there might be a json message from Web portal which begind with [{"mT":
		if (message->payloadlen > 7 && strstr((char*)message->payload,"[{\"mT\":")!=NULL)
		{
			LOGI("Web portal message for %s\n", (char*)message->payload);
			process_chat_message((unsigned char*)message->payload, message->payloadlen);
		}
		else
			process_message((unsigned char *)message->payload, message->payloadlen);
	}

}

void mqtt_client::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	owner->OnSubscribe(mid, qos_count, granted_qos);
	/*if (mid == 1)
	{
		string did= Global::Get()->GetDriverID();
		string msg =did+ "|";
		msg+= Global::Get()->GetDriverName();
		send registration message !!!
		string topic = lgn_topic + did;
		send_login_message((char*)lgn_topic.c_str(),(char*)msg.c_str());
	}*/
}

void mqtt_client::on_publish(int mid)
{
	LOGI("on_publish succeeded. mid=%d\n",mid);
	process_publish_callback(mid);
}

