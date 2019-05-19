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
#include <sys/shm.h>
#include <sys/ipc.h>

#include <sys/prctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <pwd.h>
#define ETH_NAME "wlp2s0"

int get_conf_value(char *,const char *, char *);
int socket_create(int);
int socket_connect(int , char *);
int socket_connect_nonblock(int port, char *host);
int socket_udp_server(int port, struct sockaddr_in &addr);
int socket_udp_client(int port, struct sockaddr_in &addr, const char *ip);
void get_ip(char *);
int find(const char *);
int connect_sock_test(struct sockaddr_in addr, int port);
int Write_Pi_Log(char *PihealthLog, const char *format, ...);
#ifndef _DEBUG
#define DBG(fmt, arg...) printf(fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif

#endif
