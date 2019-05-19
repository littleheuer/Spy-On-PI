/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: 2019年02月23日 星期六 10时59分17秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
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

#include <pthread.h>
#define ETH_NAME "wlp2s0"

int get_conf_value(char *,const char *, char *);
int socket_create(int);
int socket_connect(int , char *);
char* get_ip(char *);


#endif
