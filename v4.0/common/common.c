/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: 2019年02月23日 星期六 11时00分57秒
 ************************************************************************/

#include "common.h"
#define TIME_OUT_TIME 300000
//创建具有监听状态的socket套接字，监听port端口

int Write_Pi_Log(char *PiHealthLog, const char *format, ...) {
    time_t timep = time(NULL);
    struct tm *t;
    FILE *fp;
    int ret;
    va_list arg;
    va_start(arg, format);
    t = localtime(&timep);
    fp = fopen(PiHealthLog, "a+");

    if(fp == NULL) {
        DBG("error\n");
        exit(1);
    }
    int a = t->tm_year + 1990;
    int b = t->tm_mon + 1;
    int c = t->tm_mday;
    int d = t->tm_hour;
    int e = t->tm_min;
    int f = t->tm_sec;

    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d---", a, b, c, d, e, f);
    ret = vfprintf(fp, format, arg);

    fflush(fp);
    fclose(fp);
    va_end(arg);

    return ret;
}

int socket_create(int port) {
    struct sockaddr_in s_addr;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0) {
        //perror("socket create");
        return -1;
    }
    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sfd, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0) {
        //perror("socket create bind");
        close(sfd);
        return -1;
    }
    if(listen(sfd, 30) < 0) {
        //perror("listen");
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
        //perror("socketi connect");
    }
    //s_addr.sin_addr = htonl(INADDR_ANY)
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host);
    //bzero(&(serverAddr.sin_zero), sizeof(serverAddr));
    if(connect(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        //perror("socket connect->connect");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int socket_connect_nonblock(int port, char *host) {
    int sockfd = 0;
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //perror("socket nonblock connect");
        return -1;
    }
    
    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    bool ret = false;
    int error = -1, len = sizeof(int);
    struct timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);

if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if(select(sockfd + 1, NULL, &set, NULL, &tm) > 0) {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
            if(0 == error) {
                ret = true;
            } else {
                ret = false;
            }
        } else {
            ret = false;
        }
    } else ret = false;
    if(!ret) {
        close(sockfd);
        return -1;
    }
    ul = 0;
    ioctl(sockfd, FIONBIO, &ul);
    return sockfd;
}

//获取本机wlan的ip地址
void get_ip(char *ip) {
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    char *temp_ip = NULL;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        //perror("socket");
        return ;                
    }
    strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        //perror("ioctl");
        return ;
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
        DBG("fopen error\n");
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
                break;
            } else {
                continue;
            }
        }
    }
    return 0;
}



//socket,bind,udp
int socket_udp_server(int port, struct sockaddr_in &addr) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        DBG("create socket fail\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if(bind(fd, (struct sockaddr *) &addr, sizeof(addr))) {
        DBG("bind failed \n");
        return -1;
    }
    return fd;
}

//socket,udp
int socket_udp_client(int port, struct sockaddr_in &addr, const char *ip) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        DBG("create socket fail\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    return fd;
}

int find(const char *filname) {
    return 0;
}

//connect timout=.03s
int connect_sock_test(struct sockaddr_in addr, int port) {
    int fd = 0;
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //perror("socket connetc test");
        return -1;
    }
    
    int error = -1, len = sizeof(int);
    struct timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(fd, FIONBIO, &ul);
    bool ret = false;
    addr.sin_port = htons(port);

    if(connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        tm.tv_sec = 0;
        tm.tv_usec = TIME_OUT_TIME;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if(select(fd + 1, NULL, &set, NULL, &tm) > 0) {
            //信息返回在error中
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if(0 == error) ret = true;
            else ret = false;
        } else ret = false;
    } else ret = false;
    //ul = 0;
    //ioctl(fd, FIONBIO, &ul);
    if(!ret) {
        close(fd);
        return 1;
    }
    close(fd);
    DBG("connect test\n");
    return 0;
}
