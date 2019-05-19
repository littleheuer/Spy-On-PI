/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: 2019年02月23日 星期六 11时00分57秒
 ************************************************************************/

#include "common.h"

//创建具有监听状态的socket套接字，监听port端口
int socket_create(int port) {
    printf("(func out)%d\n", port);
    struct sockaddr_in s_addr;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0) {
        perror("socket");
        return -1;
    }
    //bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sfd, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0) {
        perror("bind");
        close(sfd);
        return -1;
    }
    if(listen(sfd, 30) < 0) {
        perror("listen");
        close(sfd);
        return -1;
    }
    return sfd;
}
//连接服务器connect
int socket_connect(int port, char *host) {
    int sockfd = 0;
    struct sockaddr_in serverAddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("socket");
    }
    //s_addr.sin_addr = htonl(INADDR_ANY)
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host);
    //bzero(&(serverAddr.sin_zero), sizeof(serverAddr));
    if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        printf("%d\n" , sockfd);
        perror("connect");
        close(sockfd);
        return -1;
    }
    return sockfd;
}
//获取本机wlan的ip地址
char *get_ip(char *ip) {
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    char *temp_ip = NULL;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return NULL;                
    }
    strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return NULL;
    }
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    temp_ip = inet_ntoa(sin.sin_addr);
    strcpy(ip,temp_ip);
    fprintf(stdout, "服务器ip地址(wlp2s0): %s\n", temp_ip);
}

//获取配置文件的配置
int get_conf_value(char *pathname,const char *key_name, char *value) {
    FILE *fd = NULL;
    ssize_t read = 0;
    size_t len = 0;
    char *line = NULL;
    char *substr = NULL;
    memset(value, 0, sizeof(value));
    fd = fopen(pathname, "r");
    if(fd == NULL) {
        printf("fopen error\n");
        exit(1);
    }
    while((read = getline(&line, &len, fd)) != 1) {
        substr = strstr(line, key_name);
        if(substr == NULL) {
            continue;
        } else {
            int temp = strlen(key_name);
            if(line[temp] == '=') {

                strncpy(value, &line[temp + 1], (int)read - temp - 1);
                temp = strlen(value);
                *(value + temp - 1) = '\0';
                fflush(stdin);
                DBG("(value)%s(temp)%d(read)%zu\n", value, temp, read);
                break;
            } else {
                printf("Next\n");
                continue;
            }
        }
    }
    return 0;
}

int find(const char *filename) {
    return 1;
}
