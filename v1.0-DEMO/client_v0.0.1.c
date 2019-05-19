/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月07日 星期四 14时24分29秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"

int main () {
    struct sockaddr_in addr;
    int server_port = 8731;
    char host[] = "192.168.10.190";
    char buffer[1025];
    int connect_fd = socket_connect(server_port, host);
    if(connect_fd < 0) {
        perror("connect");
        exit(1);
    }
    //FILE *file = NULL;
    //while((file = popen("cat ./common.c", "r")) == NULL) {
    //    exit(1);
    //}
    //memset(buffer, 0, sizeof(buffer));
    //while(fread(buffer, 100, 1000, file)) {
    //    send(connect_fd, buffer, sizeof(buffer), 0);
    //}
    //pclose(file);
    //close(connect_fd);
    
    return 0;
}
