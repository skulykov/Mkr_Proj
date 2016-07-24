#include "ClsMqtt.h"

#ifdef WIN32
void usleep(int d) 
{
	Sleep(d / 1000);
}
#endif

const string server_topic = "MI/";
const string login_topic = "MSG/LOGIN/";

static MqttConnStates conn_state = MqttConnStates::OPEN;
static bool initialized = false;
static bool terminate_mqtt_service = false;
static mqtt_client *mclient = 0;
//static std::queue<MessageQueue *> myqueue;
static time_t prev_time = 0, prev_hos_engine_time = 0;
static pthread_t thread = { 0 };

//public:
	char *topic;
	char *message;
	int flag;

bool mqtt_isConnected() { return conn_state == MqttConnStates::CONNECTED; }

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

void add_server_message(char *dest_id, char *msg, int flag)
{
	string my_did = Global::Get()->GetDriverID();
	string my_name = Global::Get()->GetDriverName();
	string topic = server_topic + string(dest_id);

	JsonMessageCls m(1, my_did, my_name, string(msg));
	vector<JsonMessageCls*> v;
	v.push_back(&m);
	string json = m.serializeList(v);//m.toJsonString(&m);
	mclient->publish(NULL, (char*)topic.c_str(), json.size(), json.c_str(), 1, false);   // retain flag is true

	
	LOGI("add_server_message...Sending msg:%s to topic:%s\n", (char*)msg, (char*)topic.c_str());
}

int send2server(byte *msg_payload, int size)
{
	string my_did = Global::Get()->GetDriverID();
	string topic = "SI/" + my_did + "/Q";
	int mid = 0;
	if(mclient)
		mclient->publish(&mid,(char*) topic.c_str(), size, msg_payload, 1, false);
	LOGI("send2server...Sending mid=%d msg to topic:%s\n",mid, (char*)topic.c_str());
	return mid;
}
void run_hos_engine();
static void  processComms()
{
	//LOGI("Mqtt ...Loop\n");
	time_t t = mclient->get_mqtt_time();
	if ((t - prev_time)>30)
	{
		prev_time = t;
		LOGI("MqttThread ... sending HB\n");
		SendRequest();
		try {
			run_hos_engine();

		}
		catch (std::exception &ex)
		{
			
			string str = string(ex.what());
			if (!str.empty())
			{
				LOGI("run_hos_engine()...Exc:%s\n",str.c_str());
			}
		}
		catch (...)
		{
			LOGI("run_hos_engine()...ExcALL:\n");
		}
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
}

void *doIt(void* arg)
{

	int loop_ret=0,rc1=0;

	mosqpp::lib_init();
	string my_client ="mclient-"+ Global::Get()->GetDriverID();
	mclient = new mqtt_client(my_client.c_str());
	
	conn_state = MqttConnStates::WAITING_CONN_ACK;
	mclient->mqtt_connect( /*"184.94.19.3"*//*"localhost"*/"23.95.120.21", 1883);

	prev_time=mclient->get_mqtt_time();
	prev_hos_engine_time = prev_time;

	terminate_mqtt_service = false;
	while (!terminate_mqtt_service) {
		loop_ret = mclient->loop();
		if (loop_ret) {
			LOGI(" doIt..mclient->loop error %d.\n", loop_ret);
			mclient->reconnect();
		}

#if 0
		//LOGI("GOINT TO LOOP LIBRARY !!!\n");
		//if (!myqueue.empty())
		//{
		//	MessageQueue *mq = myqueue.front();
		//	send_message(mq->topic, mq->message);
		//	myqueue.pop();  // we need to wait until confirmation comes !!!
		//}
		loop_ret = mclient->loop();
		if (loop_ret) {
			LOGI(" doIt..mclient->loop error %d.\n", loop_ret);
			usleep(1000 * 1000);
			//conn_state = MqttConnStates::ERROR_LOOP;
			if (conn_state == MqttConnStates::DISCONNECTED)
			{
				LOGI(" doIt..Trying to Reconnect...\n");
				rc1 = mclient->reconnect();
				if (rc1 == 0)
				{
					conn_state = MqttConnStates::WAITING_CONN_ACK;
					LOGI(" doIt..MqttConnStates::WAITING_CONN_ACK...\n");
				}
				else {
					conn_state = MqttConnStates::ERROR_ON_RECONNECT;

				}
			}
			if (loop_ret == MOSQ_ERR_NO_CONN || loop_ret == MOSQ_ERR_CONN_REFUSED || loop_ret == MOSQ_ERR_CONN_LOST)
			{
				conn_state = MqttConnStates::DISCONNECTED;
			}
			usleep(1000 * 1000);
		}
		switch (conn_state)
		{
		case MqttConnStates::UNKNOWN:
			LOGI(" doIt..MqttConnStates::MqttConnStates::UNKNOWN...\n");
			usleep(1000 * 1000);
			
			break;
		case MqttConnStates::ERROR_LOOP:
		{
			
		}
			break;
		case MqttConnStates::ERROR_ON_RECONNECT:
			LOGI(" doIt..MqttConnStates::ERROR_ON_RECONNECT...\n");
			usleep(1000 * 1000);
			break;
		case  MqttConnStates::ON_CONNECTED:
		{
			//if (conn_state == MqttConnStates::WAITING_CONN_ACK)
			{
				int mid = 1;// , mid1 = 2;
				string topic = server_topic + Global::Get()->GetDriverID();    // subscribe to get responses from server  
				int rc = mclient->subscribe(&mid, topic.c_str());
				LOGI("Mqtt ...Sending subscribe to topic:%s mid=%d\n", topic.c_str(), mid);
				if (rc == 0)
				{
					conn_state = MqttConnStates::SUBSCRIBE_1_SENT;
				}
			}
		}
			break;
		case MqttConnStates::SUBSCRIBE_1_ACK:
		{
			int mid = 2;// , mid1 = 2;
			string topic = login_topic+"#";// +Global::Get()->GetDriverID();    // subscribe to get responses from server  
			int rc = mclient->subscribe(&mid, topic.c_str());
			LOGI("Mqtt ...Sending subscribe to topic:%s\n", topic.c_str());
			if (rc == 0)
			{
				conn_state = MqttConnStates::SUBSCRIBE_2_SENT;
			}
		}
			break;
		case MqttConnStates::SUBSCRIBE_2_ACK:
		{
			conn_state = MqttConnStates::CONNECTED;
			LOGI(" doIt..MqttConnStates::CONNECTED.\n");
			string my_did = Global::Get()->GetDriverID();
			string my_name = Global::Get()->GetDriverName();
			string topic = login_topic + my_did;
			int mid = 0;
			if (mclient)
			{
				//[{"mT":1, "dId" : "admin_3604", "dN" : "Dispatcher", "msg" : "1", "dt" : "2016-05-05T14:51:55.092Z"}]
				JsonMessageCls m(1, my_did,my_name,"1");
				vector<JsonMessageCls*> v;
				v.push_back(&m);
				string json = m.serializeList(v);//m.toJsonString(&m);
				mclient->publish(&mid, (char*)topic.c_str(), json.size(), json.c_str(), 1, true);   // retain flag is true
				LOGI("Mqtt ...Sending mid=%d msg to topic:%s\n", mid, (char*)topic.c_str());
			}
		}
			break;
		case MqttConnStates::CONNECTED:
			//processComms();
			break;
		}
#endif		
		usleep(10 * 1000);                                //sleep(seconds) or usleep(microseconds)
		
	}
	LOGI("Clean Mqtt ...\n");
	if (mclient)
		delete mclient;
	mosqpp::lib_cleanup();
	initialized = false;
	return 0;
}
void  doInitMqtt()
{
	if (initialized)
	{
		LOGI("MQTT thread init=1... \n");
		return;
	}
	
		
		if (pthread_create(&thread, NULL, doIt, 0) != 0)
		{
			LOGI( "ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");
			
		}
		initialized = true;
	
}
void stopMqtt()
{
	terminate_mqtt_service = true;
}
//------------------------------------------------------------
void MOnConnect(int rc)
{
	LOGI("Connected with code %d.\n", rc);
	
	if (rc == 0) {
		conn_state = MqttConnStates::ON_CONNECTED;
	}
	else
		conn_state = MqttConnStates::UNKNOWN;
}
void MOnDisconnect(int rc)
{
	if (rc == MOSQ_ERR_SUCCESS) {}
	LOGI("OnDisconnect..!!.rc=%d\n", rc);
	conn_state = MqttConnStates::DISCONNECTED;
}
void MOnSubscribe(int mid, int qos_count, const int *granted_qos)
{
	LOGI("Subscription succeeded.mid=%d\n", mid);
	if (/*mid == 1 &&*/ conn_state == MqttConnStates::SUBSCRIBE_1_SENT)
		conn_state = MqttConnStates::SUBSCRIBE_1_ACK;
	else
		if (/*mid == 2 &&*/ conn_state == MqttConnStates::SUBSCRIBE_2_SENT)
			conn_state = MqttConnStates::SUBSCRIBE_2_ACK;
		else
			conn_state = MqttConnStates::UNKNOWN;
}

/*
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
*/