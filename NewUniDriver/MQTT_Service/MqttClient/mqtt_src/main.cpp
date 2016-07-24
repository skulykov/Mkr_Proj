#include "temperature_conversion.h"

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main_thread();

void setMM(int c);

#define MAX_LINE 23

char *Readline(char *in) {
	char *cptr;

	if (cptr = fgets(in, MAX_LINE, stdin)) {
		// kill preceding whitespace but leave \n so we're guaranteed to have something
		while(*cptr == ' ' || *cptr == '\t') {
		cptr++;
		}
		return cptr;    
		} else {
		return 0;
		}
}

int main(int argc, char *argv[])
{

	
#if 0
	
	int rc;

	mosqpp::lib_init();

	tempconv = new mqtt_tempconv("tempconv", /*"184.94.19.3"*//*"localhost"*/"23.95.120.21", 1883);
	
	while(1){
		rc = tempconv->loop();
		if(rc){
			tempconv->reconnect();
		}
	}

	mosqpp::lib_cleanup();
#endif
	main_thread();;
	

	printf("\r\npress q to exit..\r\n");
	//===========================
	
	//===============================
	

	char msg[128];
	for (;;) {
		if (_kbhit())
		{
			int c = _getch();//fgetc(stdin);
			setMM(c);

			if (c == 'q')
				break;
		}
	}



	return 0;
}

