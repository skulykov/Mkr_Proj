#include "ThreadMqtt.h"
#include "my_time.h"
#include "Dbg.h"
#include "Global.h"
#include <queue>          // std::queue
#include "../network/NetComm.h"
#include "../BizLogic/hos/hos_interface.h"

bool initialized = false, terminate_mqtt_service = false;

static class mqtt_client *mclient=0;
//static std::queue<MessageQueue *> myqueue;

string getPrefix();

//void add_message(char *dest_id, char *msg_payload,int flag)
//{
//	string pref= getPrefix();
//	string topic = pref + string(dest_id);
//	string my_did = Global::Get()->GetDriverID();
//	string msg = my_did + "|" + string(msg_payload);
//	MessageQueue *mq = new MessageQueue((char*)topic.c_str(), (char*)msg.c_str(), flag);
//	myqueue.push(mq);
//}


void send_message(char *topic, char *msg_payload)
{
	if(mclient)
		mclient->publish(NULL, topic, strlen(msg_payload), msg_payload, 1, false);
}
void send_login_message(char *topic, char *msg_payload)
{
	if(mclient)
		mclient->publish(NULL, topic, strlen(msg_payload), msg_payload, 1, true); // retain
}

void add_server_message(char *dest_id, char *msg_payload, int flag)
{
	string my_did = Global::Get()->GetDriverID();
	string topic = "SI/" + my_did + "/Q";

	string msg = my_did + "|" + string(msg_payload);
	//MessageQueue *mq = new MessageQueue((char*)topic.c_str(), (char*)msg.c_str(), flag);
	//myqueue.push(mq);

	send_message((char*)topic.c_str(), (char*)msg.c_str());
	LOGI("Mqtt ...Sending msg:%s to topic:%s\n", (char*)msg.c_str(), (char*)topic.c_str());
}

int send2server(byte *msg_payload, int size)
{
	string my_did = Global::Get()->GetDriverID();
	string topic = "SI/" + my_did + "/Q";
	int mid = 0;
	if(mclient)
		mclient->publish(&mid,(char*) topic.c_str(), size, msg_payload, 1, false);
	LOGI("Mqtt ...Sending mid=%d msg to topic:%s\n",mid, (char*)topic.c_str());
	return mid;
}

static time_t prev_time = 0, prev_hos_engine_time = 0;
static void *doIt(void* _arg)
{

	int rc;

	mosqpp::lib_init();

	mclient = new mqtt_client("mclient", /*"184.94.19.3"*//*"localhost"*/"23.95.120.21", 1883);
	terminate_mqtt_service = false;
	while (!terminate_mqtt_service) {
		//LOGI("GOINT TO LOOP LIBRARY !!!\n");
		//if (!myqueue.empty())
		//{
		//	MessageQueue *mq = myqueue.front();
		//	send_message(mq->topic, mq->message);
		//	myqueue.pop();  // we need to wait until confirmation comes !!!
		//}
		rc = mclient->loop();
		if (rc) {
			LOGI(" doIt..ReConnect with code %d.\n", rc);
			mclient->reconnect();
		}
		//LOGI("Mqtt ...Loop\n");
		time_t t = mclient->get_mqtt_time();
		if ((t - prev_time)>30)
		{
			prev_time = t;
			LOGI("MqttThread ... sending HB\n");
			SendRequest();
		}
		if ((t - prev_hos_engine_time)>60)
		{
			prev_hos_engine_time = t;

			process_all_unsent_from_sqlite(0);
			LOGI("MqttThread ... HOS Engine\n");
			
		}
		/*int sts = get_signal_to_send();
		if (sts>0)
		{
			process_all_unsent_from_sqlite(sts);
		}*/
		
		usleep(10*1000);                                //sleep(seconds) or usleep(microseconds)
	}
	LOGI("Clean Mqtt ...\n");
	mosqpp::lib_cleanup();

	return 0;
}
void  doInitMqtt(int par1, char *payload)
{
	if (initialized)
	{
		LOGI("MQTT thread init=1... \n");
		return;
	}
		
	MqttThreadRequest_T *arg = new MqttThreadRequest_T();

	arg->par1 = par1;
	arg->par2 = 2;
	arg->payload = payload;
	arg->cb = 0;

	//createRenderingThread
	{
		pthread_t thread = { 0 };
		if (pthread_create(&thread, NULL, doIt, (void *)arg) != 0)
		{
			LOGI( "ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");

			if (arg != 0)
			{
				delete arg;
			}
			if (payload != 0)
				delete payload;//free(payload);

			initialized = true;
		}
	}
}
void stopMqtt()
{
	terminate_mqtt_service = true;
}

//---------------Use mutex---------
int signal_to_send = 0;
pthread_mutex_t stopMutex;
int get_signal_to_send(void) {
	int ret = 0;
	pthread_mutex_lock(&stopMutex);
	ret = signal_to_send;
	signal_to_send = 0;                 // svk  Not sure ??
	pthread_mutex_unlock(&stopMutex);
	return ret;
}

void setsignal_to_send(int val) {
	pthread_mutex_lock(&stopMutex);
	signal_to_send = val;
	pthread_mutex_unlock(&stopMutex);
}