#ifndef MKR_CLSMQTT_H_
#define MKR_CLSMQTT_H_

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
#include "JsonMessageCls.h"
void process_publish_callback(int mid);  // defined in NetComm.cpp

enum MqttConnStates
{
	UNKNOWN,
	ERROR_LOOP,
	OPEN,
	CONNECT,
	RECONNECT,
	WAITING_CONN_ACK,
	ERROR_ON_RECONNECT,
	ON_CONNECTED,
	SUBSCRIBE_1_SENT,
	SUBSCRIBE_1_ACK,
	SUBSCRIBE_2_SENT,
	SUBSCRIBE_2_ACK,
	CONNECTED,
	DISCONNECTED
};

using namespace mosqpp;
using namespace std;

class ClsMqtt;
class mqtt_client : public mosquittopp
{
	class ClsMqtt *owner=0;

	void process_message(unsigned char *payload, int len);
	void process_chat_message(unsigned char *payload, int len,bool login_flag=false);
public:
	mqtt_client();
	~mqtt_client();

	mqtt_client(const char *id);
	int mqtt_connect(const char *host, int port);
	void setOwner(ClsMqtt *o) { owner = o; }

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
	void on_publish(int mid);

};


typedef void(*cbMqttThreadFunc_T)(void);
typedef struct {

	int par1;
	float par2;
	char *payload;
	cbMqttThreadFunc_T cb;
}MqttThreadRequest_T;

class ClsMqtt 
{
	MqttConnStates conn_state= MqttConnStates::OPEN;
	bool initialized = false;
	bool terminate_mqtt_service = false;

	mqtt_client *mclient = 0;
	//static std::queue<MessageQueue *> myqueue;
	time_t prev_time = 0, prev_hos_engine_time = 0;

	pthread_t thread = { 0 };

	//string getPrefix();

	static ClsMqtt *this_instance;

public:
	char *topic;
	char *message;
	int flag;
	ClsMqtt() 
	{
		
	}
	~ClsMqtt()
	{
		
	}
	static ClsMqtt *Get() 
	{ 
		if (this_instance == 0)
			this_instance = new ClsMqtt();
		return this_instance; 
	}
	void *doIt();

	static void *thread_helper_function(void *context)
	{
		ClsMqtt *c=(ClsMqtt *)context;
		return c->doIt();
	}

	//-----------------------------------------------
	bool isConnected() { return conn_state == MqttConnStates::CONNECTED; }
	void send_message(char *topic, char *msg_payload);
	void send_login_message(char *topic, char *msg_payload);
	void add_server_message(char *dest_id, char *msg_payload, int flag);
	int send2server(byte *msg_payload, int size);
	void  doInitMqtt(int par1, char *payload);
	void stopMqtt();

	void processComms();
	void OnConnect(int rc);
	void OnDisconnect(int rc);
	void OnSubscribe(int mid, int qos_count, const int *granted_qos);
};

/*
int get_signal_to_send(void);
void setsignal_to_send(int val);
*/
void JniApi_PostReceivedMsgProto(int req, int par, void *payload, int payload_len);
#endif
