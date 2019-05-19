/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月07日 星期四 14时24分29秒
 ************************************************************************/

#include "common.h"

char script[3][20] = {"./CpuLog.sh", "./DiskLog.sh", "./memory.sh 20"};
char log_name[3][20] = {"mem.log", "disk.log", "cpu.log"};
char conf_name[] = "./default.conf";
int master_port, client_port, sig_port, file_port;
char host[50], temp[7];
char buffer[1025];

//执行脚本生成日志
void *shell(void *) {
  
    char buffer[1024] = {0};
    while(1) {
        FILE *file = NULL;
        char exec[50] = {0};
        for(int i = 0; i < 3; i++) {
            memset(exec, 0, sizeof(exec));
            int n = 10;
            while((file = popen(script[i], "r")) == NULL && (--n)) {
                exit(1);
            }
            memset(buffer, 0, sizeof(buffer));
            FILE *log_fp;
            log_fp = fopen(log_name[i], "a+");
            if(log_fp == NULL) {
                perror("log_fp");
                exit(1);
            }

            while(fread(buffer, 100, 1000, file)) {
                fprintf(log_fp, "%s\n", buffer);
            }
            fclose(log_fp);
            pclose(file);
        }
        sleep(10);
    }
}

//文件发送函数
int send_file(int master_port) {
    for(int i = 0; i < 5; i++) {
        int fd;
        if((fd = socket_connect(master_port, host)) < 0) {
            printf("8733 connect failed\n");
            return 1;
        }
        char buffer[1024] = {0};
        memset(buffer, 0, sizeof(buffer));
        send(fd, log_name[i], sizeof(log_name[i]), 0);
        FILE *fp = NULL;
        fp = fopen(log_name[i], "r");
        if(fp == NULL) {
            printf("%s file open failed\n", log_name[i]);
            return 1;
        }
        int real_len = 0;
        while ((real_len = fread(buffer, 100, 1000, fp)) > 0) {
            send(fd, buffer, real_len, 0);
            memset(buffer, 0, sizeof(buffer));
        }
        fclose(fp);
    }
    return 0;
}

//文件发送 8733端口监听file_port
void *client_send(void *) {
    int listen_fd = socket_create(file_port);
    if(listen_fd < 0) {
        printf("client_send fd create failed\n");
        return NULL;
    }
    while (1) {
        int fd;
        if((fd = accept(listen_fd, NULL, NULL)) < 0) {
            printf("accept faild\n");
            close(listen_fd);
            continue;
        }
        recv(fd, buffer, sizeof(buffer), 0);
        if(strcmp(buffer, "1")) {
            send_file(master_port);
        }
    }
}

//心跳检测 10000端口监听client_port
void *heart_recv(void *) {
    int heart_fd =socket_create(client_port);
    if(heart_fd < 0) {
        perror("heart_recv");
        return NULL;
    }
    int sfd;
    while (1) {
        sfd = accept(client_port, NULL, NULL);
        if(sfd < 0) {
            perror("accept 1");
            close(sfd);
            continue;
        
        }
        close(sfd);
    }
}


//signal 9000 端口监听sig_port
void *sig(void *) {
    int sig_fd = 0;
    if((sig_fd = socket_create(sig_port)) < 0) {
        perror("sig");
        return NULL;
    }
    int msg_fd;
    struct sockaddr_in s_addr;
    char buffer[1024] = {0};
    while (1) {
        int len = sizeof(s_addr);
        msg_fd = accept(sig_fd, (struct sockaddr *) &s_addr, (socklen_t *) &len);
        if(msg_fd < 0) {
            perror("msg_fd");
            continue;
        }
        int data_num;
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            data_num = recv(msg_fd, buffer, sizeof(buffer), 0);
            if(data_num < 0) {
                perror("recv NULL");
                continue;
            }
            buffer[data_num] = '\0';
            if(data_num <= 0) {
                close(msg_fd);
                break;
            }
            int ans;
            if(strcmp(buffer, "100")) {
                ans = find(script[0]);
                if(ans) {
                    sprintf(buffer, "200");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                } else {
                    sprintf(buffer, "400");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                }
            } else if(strcmp(buffer, "101")) {
                ans = find(script[2]);
                if(ans) {
                    sprintf(buffer, "201");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                } else {
                    sprintf(buffer, "401");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                }
                
            } else if(strcmp(buffer, "102")) {
                ans = find(script[1]);
                if(ans) {
                    sprintf(buffer, "202");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                } else {
                    sprintf(buffer, "402");
                    send(msg_fd, buffer, sizeof(buffer), 0);
                }
            }
        }
    }
}

int main () {
    struct sockaddr_in addr;
    get_conf_value(conf_name, "master_port", temp);
    master_port = atoi(temp);
    get_conf_value(conf_name, "sig_port", temp);
    sig_port = atoi(temp);
    get_conf_value(conf_name, "file_port", temp);
    file_port = atoi(temp);
    get_conf_value(conf_name, "client_port", temp);
    client_port = atoi(temp);
    //get_ip(host);
    sprintf(host, "192.168.2.103");
    
    pthread_t log_write, send_t, heart_t, sig_t;

    pthread_create(&log_write, NULL, shell, NULL);
    pthread_create(&send_t, NULL, client_send, NULL);
    pthread_create(&heart_t, NULL, heart_recv, NULL);
    pthread_create(&sig_t, NULL, sig, NULL);
    int connect_fd = socket_connect(master_port, host);
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
    close(connect_fd);
     
    pthread_join(send_t, NULL);
    pthread_join(log_write, NULL);
    pthread_join(heart_t, NULL);
    pthread_join(sig_t, NULL);
    return 0;
}
