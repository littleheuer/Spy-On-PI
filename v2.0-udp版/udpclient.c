/*************************************************************************
	> File Name: udpclient.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月19日 星期二 19时51分18秒
 ************************************************************************/

#include<stdio.h>
#include "./UDP.h"
#define server_port 8731
#define udp_server_port "192.168.2.103"

int main () {
    int c_fd;
    struct sockaddr_in addr;
    c_fd = socket_udp_client(server_port, addr, udp_server_port);
    char buffer[1024] = {0};
    int len;
    socklen_t addr_len = sizeof(addr);
    while (1) {
        scanf("%s", buffer);
        sendto(c_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
        len = recvfrom(c_fd, buffer,sizeof(buffer), 0, (struct sockaddr *) &addr, &addr_len);
        printf("server:%s\n", buffer);
        sleep(2);
    }
    close(c_fd);
}
