/*************************************************************************
	> File Name: UDP.h
	> Author: 
	> Mail: 
	> Created Time: 2019年03月19日 星期二 19时59分36秒
 ************************************************************************/

#ifndef _UDP_H
#define _UDP_H

#include<stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>    
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include <sys/prctl.h>

#include <pthread.h>

int socket_udp_server(int port, struct sockaddr_in &addr);
int socket_udp_client(int port, struct sockaddr_in &addr, const char *ip);
#endif
