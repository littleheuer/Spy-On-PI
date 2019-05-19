/*************************************************************************
	> File Name: udpmaster.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月19日 星期二 20时26分51秒
 ************************************************************************/

#include<stdio.h>
#include"../common.h"
#include"./UDP.h"
#define port 8731
int main () {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char buffer[1024] = {0};
    int s_fd = socket_udp_server(port, addr);
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        recvfrom(s_fd, buffer, sizeof(buffer), 0,(struct sockaddr *) &addr, &len);
        printf("client:%s\n", buffer);
        scanf("%s", buffer);
        sendto(s_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, sizeof(addr));
        sleep(2);
    }
    close(s_fd);
    return 0;
}
