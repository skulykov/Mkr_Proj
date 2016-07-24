// stdafx.cpp : source file that includes just the standard includes
// MqttProj.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "temperature_conversion.h"
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

// For WinXp as a target, change accordingly

#define _WIN32_WINNT 0x0501
void send_message(char *topic, char *msg);


#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include <pthread.h>

class mqtt_tempconv *tempconv;
int getMM(int c)
{
	if (c == 'a')
	{
		printf("\r\nEnter message..\r\n");

		//memset(msg, 0, sizeof(msg));
		//Readline(msg);
		send_message("Devin/Login", "Message To Login");
		return 1;
	}
	if (c == 'b')
	{
		printf("\r\nEnter message..\r\n");

		//memset(msg, 0, sizeof(msg));
		//Readline(msg);
		send_message("Devin/Chat", "Message To Chat");
		return 1;
	}
	return 0;
}
int gChar = 0;
void setMM(int c)
{
	gChar = c;
}
void doIt()
{
	
	int rc;

	mosqpp::lib_init();

	tempconv = new mqtt_tempconv("tempconv", /*"184.94.19.3"*//*"localhost"*/"23.95.120.21", 1883);

	while (1){
		//printf("GOINT TO LOOP LIBRARY !!!\n");
		if (getMM(gChar))
		{
			gChar = 0;
		}
		rc = tempconv->loop();
		if (rc){
			printf("Reconnect.. !!!\n");
			tempconv->reconnect();
		}
		printf(".");
		Sleep(10);
	}
	printf("CLOSING LIBRARY !!!\n");
	mosqpp::lib_cleanup();
}

void* MyThreadFunction(void* lpParam)
{

	printf("Entering doIT...\n");
	doIt();
	return 0;

}

#include "temperature_conversion.h"

pthread_t thread = { 0 };
int main_thread()
{

	if (pthread_create(&thread, NULL, MyThreadFunction, 0) != 0)
	{
		printf("ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");

											/*if (arg != 0)
											{
											delete arg;
											}*/
		
	}

	return 0;

}

//const char *msg_payload = "XXXabcdewsaqwYYY";
void send_message(char *topic,char *msg_payload)
{
	tempconv->publish(NULL, topic, strlen(msg_payload), msg_payload, 1, false);
}

