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

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{

	printf("The parameter: %u.\n", *(DWORD*)lpParam);
	doIt();
	return 0;

}

#include "temperature_conversion.h"

int main_thread()
{

	DWORD dwThreadId, dwThrdParam = 1;

	HANDLE hThread;



	hThread = CreateThread(

		NULL, // default security attributes

		0, // use default stack size 

		MyThreadFunction, // thread function

		&dwThrdParam, // argument to thread function

		0, // use default creation flags

		&dwThreadId); // returns the thread identifier



	printf("The thread ID: %d.\n", dwThreadId);



	// Check the return value for success. If something wrong...

	if (hThread == NULL)

		printf("CreateThread() failed, error: %d.\n", GetLastError());

	//else, gives some prompt...

	else

		printf("It seems the CreateThread() is OK lol!\n");

	if (CloseHandle(hThread) != 0)

		printf("Handle to thread closed successfully.\n");

	return 0;

}

//const char *msg_payload = "XXXabcdewsaqwYYY";
void send_message(char *topic,char *msg_payload)
{
	tempconv->publish(NULL, topic, strlen(msg_payload), msg_payload, 1, false);
}

