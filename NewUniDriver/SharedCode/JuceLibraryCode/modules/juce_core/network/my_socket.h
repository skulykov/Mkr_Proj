#ifndef MY_SOCKET_H_INCLUDED
#define MY_SOCKET_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


int m_connect(char *hostname, int port);




#endif
