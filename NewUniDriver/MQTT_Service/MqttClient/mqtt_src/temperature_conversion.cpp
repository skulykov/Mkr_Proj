#include <cstdio>
#include <cstring>

#include "temperature_conversion.h"
#include <mosquittopp.h>

mqtt_tempconv::mqtt_tempconv(const char *id, const char *host, int port) : mosquittopp(id)
{
	int keepalive = 60;

	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);
};

mqtt_tempconv::~mqtt_tempconv()
{
}

void mqtt_tempconv::on_connect(int rc)
{
	printf("Connected with code %d.\n", rc);
	if(rc == 0){
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, /*"temperature/celsius"*/"MSG/LOGIN/#");
		subscribe(NULL, /*"temperature/celsius"*/"MI/1000");
	}
}
void mqtt_tempconv::on_disconnect(int rc)
{
	printf("DISConnected with code %d.\n", rc);
	if (rc == 0) {
		/* Only attempt to subscribe on a successful connect. */
		
	}
}

void mqtt_tempconv::on_message(const struct mosquitto_message *message)
{
	double temp_celsius, temp_farenheit;
	char buf[51];
	printf("on message topic: %s\n", message->topic);
	if(!strcmp(message->topic, /*"temperature/celsius"*/"Devin/Login")){
		/*
		memset(buf, 0, 51*sizeof(char));
		// Copy N-1 bytes to ensure always 0 terminated. 
		memcpy(buf, message->payload, 50*sizeof(char));
		temp_celsius = atof(buf);
		temp_farenheit = temp_celsius*9.0/5.0 + 32.0;
		//snprintf(buf, 50, "%f", temp_farenheit);
		sprintf(buf,  "%f", temp_farenheit);
		publish(NULL, "temperature/farenheit", strlen(buf), buf);
		*/
		printf("\r\non_message  %s\r\n", message->payload);
	}
}

void mqtt_tempconv::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("Subscription succeeded.\n");
}

void mqtt_tempconv::on_publish(int mid)
{
	printf("on_publish succeeded.\n");
}
