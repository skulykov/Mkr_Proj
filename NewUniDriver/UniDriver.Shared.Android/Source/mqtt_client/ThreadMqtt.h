#ifndef MKR_THREADMQTT_H_
#define MKR_THREADMQTT_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "my_types.h"

#include "mqtt_client.h"
#include <mosquittopp.h>

typedef void(*cbMqttThreadFunc_T)(void);
typedef struct {

	int par1;
	float par2;
	char *payload;
	cbMqttThreadFunc_T cb;
}MqttThreadRequest_T;

class MessageQueue
{
public:
	char *topic;
	char *message;
	int flag;
	MessageQueue(char *topic, char *msg, int flag)
	{
		this->flag = flag;
		int len = strlen(topic);
		this->topic = new char[len + 1];
		strcpy(this->topic, topic);

		len = strlen(msg);
		this->message = new char[len + 1];
		strcpy(this->message, msg);
	}
	~MessageQueue()
	{
		if (this->topic != 0)
			delete[] this->topic;
		if (this->message != 0)
			delete[] this->message;
	}
};


void  doInitMqtt(int par1, char *payload);
void stopMqtt();

void add_message(char *topic, char *msg_payload, int flag);

void add_server_message(char *topic, char *msg_payload, int flag);
int send2server(byte *msg_payload, int size);

int get_signal_to_send(void);
void setsignal_to_send(int val);

#endif
