/*************************************************************************
	> File Name: epoll-client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月04日 星期四 19时53分18秒
 ************************************************************************/

#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

int main () {
    int fd;
    struct sockaddr_in addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("socket");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7731);
    addr.sin_addr.s_addr = inet_addr("192.168.1.40");
    if(connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    char buff[400] = {0};
    while(scanf("%s", buff) != EOF) {
        send(fd, buff, strlen(buff), 0);
        memset(buff, 0, sizeof(buff));
        recv(fd, buff, sizeof(buff), 0);
        printf("%s\n", buff);

    }
    return 0;
}
