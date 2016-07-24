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


	
	//-----------------------------------------------
	bool mqtt_isConnected();
	void send_message(char *topic, char *msg_payload);
	void send_login_message(char *topic, char *msg_payload);
	void add_server_message(char *dest_id, char *msg_payload, int flag);
	int send2server(byte *msg_payload, int size);
	void  doInitMqtt();
	void stopMqtt();

	
	void MOnConnect(int rc);
	void MOnDisconnect(int rc);
	void MOnSubscribe(int mid, int qos_count, const int *granted_qos);


/*
int get_signal_to_send(void);
void setsignal_to_send(int val);
*/
void JniApi_PostReceivedMsgProto(int req, int par, void *payload, int payload_len);
#endif
