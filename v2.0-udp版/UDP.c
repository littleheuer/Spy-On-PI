/*************************************************************************
	> File Name: UDP.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月19日 星期二 20时00分28秒
 ************************************************************************/

#include<stdio.h>
#include"UDP.h"

//socket,bind
int socket_udp_server(int port, struct sockaddr_in &addr) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        printf("create socket fail\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if(bind(fd, (struct sockaddr *) &addr, sizeof(addr))) {
        printf("bind failed \n");
        return -1;
    }
    return fd;
}

int socket_udp_client(int port, struct sockaddr_in &addr, const char *ip) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        printf("create socket fail\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    return fd;
}
