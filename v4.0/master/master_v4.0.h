/*************************************************************************
	> File Name: master.h
	> Author: 
	> Mail: 
	> Created Time: Sat 09 Mar 2019 18:08:47 CST
 ************************************************************************/
#include "../common/common.h"
#include <mysql/mysql.h>
#define TIME_OUT_TIME 300000

char PiHealthLog[] = "./PiHealth.log";

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct Node{
    struct sockaddr_in client_addr;
    struct Node *next;
    int fd;
}Node, *LinkedList;

struct PRINT{
    LinkedList head;
    int index;
}; 

struct HEART{
    LinkedList *list;
    int ins;
    int *sum;
    int port_C;
};

//处理ip字段为数字
int transip(char *sip, int *ip) {
    if(sip == NULL) return -1;
    char temp[4];
    int count = 0;
    while (1) {
        int index = 0;
        while(*sip != '\0' && *sip != '.' && count < 4) {
            temp[index++] = *sip;
            sip++;
        }
        if (index == 4) return -1;
        temp[index] = '\0';
        ip[count] = atoi(temp);
        count++;
        if (*sip == '\0') {
            if (count == 4) return 0;
        } else {
            sip++;
        }
    }
    return 0;
}


int insert(LinkedList head, Node *node) {
    Node *p;
    pthread_mutex_lock(&list_mutex);
    p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = node;
    pthread_mutex_unlock(&list_mutex);
    return 1;
}

//查找最短链
int find_min(int *sum, int ins) {
    int ans = 0;
    for (int i = 0; i < ins; i++) {
        if (*(sum +i) < *(sum + ans)) {
            ans = i;
        }
    }
    return ans;
}

//连接数据库
MYSQL* connect_db(MYSQL *ptr, const char *host, const char *user, const char *pwd, const char *db) {
    ptr = mysql_init(NULL);
    if(ptr == NULL) {
        return NULL;
    }
    ptr = mysql_real_connect(ptr, host, user, pwd, db, 0, NULL, 0);
    if(ptr == NULL) {
        Write_Pi_Log(PiHealthLog, "connect error:%d,%s\n", mysql_errno(ptr), mysql_error(ptr));
        DBG("connect error:%d,%s\n", mysql_errno(ptr), mysql_error(ptr));
        return NULL;
    }
    Write_Pi_Log(PiHealthLog, "connect mysql database success\n");
    DBG("connect database success\n");
    return ptr;
}

//插入警告信息到数据库中
int insert_warning(char *buffer, MYSQL *ptr) {
    int rs = 0;
    rs = mysql_real_query(ptr, buffer, strlen(buffer));
    if(0 != rs) {
        Write_Pi_Log(PiHealthLog, "mysql_real_query():%s\n", mysql_error(ptr));
        printf("mysql_real_query():%s\n", mysql_error(ptr));
        return 0;
    }
    return 1;
}
//记录ip到log文件里
void *print(void *arg) {
    struct PRINT *print_para = (struct PRINT *)arg;
    int index = print_para->index;
    char filename[50] = {0};
    sprintf(filename, "./%d.log", index);
    int temp = 0;
    while (1) {
        FILE *file = fopen(filename, "w");
        
        pthread_mutex_lock(&list_mutex);
        Node *p = print_para->head;
        while (p -> next != NULL) {
            p = p->next;
            fprintf(file, "%s:%d\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
        }
        pthread_mutex_unlock(&list_mutex);
        
        fclose(file);
        sleep(3);
    }

    return NULL;
}

//查重
int check(LinkedList *head, struct sockaddr_in addr, int ins) {
    for(int i = 0; i < ins; i++) {
        pthread_mutex_lock(&list_mutex);
        Node *p =head[i];
        while(p && p->next) {
            p = p->next;
            if(addr.sin_addr.s_addr == p->client_addr.sin_addr.s_addr) {
                
                DBG("%s**in list**\n", inet_ntoa(addr.sin_addr));
                pthread_mutex_unlock(&list_mutex);
                return 1;
            }
        }
        pthread_mutex_unlock(&list_mutex);
    }
    return 0;
}

//非阻塞
void *heart_beat(void *arg) {
    int max_fd = 0;

    struct HEART *heart = (struct HEART *) arg;
    DBG("heart ind=%d\n", heart->ins);
    int arr[heart->ins + 1][1024] = {0};
    while (1) {
        memset(arr, 0, sizeof(arr));
        Node *p = NULL, *pre = NULL;
        
        fd_set r_set, w_set;
        FD_ZERO(&r_set);
        FD_ZERO(&w_set);
        max_fd = 0; 
        pthread_mutex_lock(&list_mutex);
        for(int i = 0; i < heart->ins; i++) {
            p = heart->list[i];
            while(p && p->next) {
                pre = p;
                p = p->next;
                
                int fd;
                fd = socket(AF_INET, SOCK_STREAM, 0);
                unsigned long ul = 1;
                ioctl(fd, FIONBIO, &ul);
                p->fd = fd;
                int stat;
                if(connect(fd, (struct sockaddr *)&p->client_addr, sizeof(p->client_addr)) == 0) {
                    perror("heart connect");
                }
                FD_SET(fd, &r_set);
                FD_SET(fd, &w_set);
                //arr[i][0]存长度
                arr[i][++arr[i][0]] = fd;
                if(fd > max_fd) max_fd = fd;
            }
        }
        struct timeval tm;
        tm.tv_sec = 5;
        tm.tv_usec = 0;
        usleep(700000);
        select(max_fd + 1, &r_set, &w_set, NULL, &tm);
        for(int i = 0; i < heart->ins; i++) {
            for(int j = 1; j <= arr[i][0]; j++) {
                close(arr[i][j]);

                p = heart->list[i];
                while (p && p->next) {
                    pre = p;
                    p = p->next;
                    if(p->fd == arr[i][j]) break;
                }
                int r = FD_ISSET(arr[i][j], &r_set);
                int w = FD_ISSET(arr[i][j], &w_set);
                //DBG("w=%d r=%d\n", w, r);
                if((!w && !r) || (w && r)) {
                    int error = -1, len = sizeof(int);
                    getsockopt(arr[i][j], SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
                    if(0 != error) {
                    
                        DBG("%s:%d not online\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                        DBG("\033[33m%s:%d deleting\033[0m\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                        pre->next = p->next;
                        free(p);
                        p = pre;
                        heart->sum[i]--;
                    } else {
                        DBG("\033[32m%s:%d online\033[0m\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                    }
                }
                else if(w && !r) {
                        DBG("\033[32m%s:%d online\033[0m\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                }
            }
        }
        pthread_mutex_unlock(&list_mutex);
        sleep(5);
    }
    return NULL;
}

void *heart_beat_bak(void *arg) {
    int arr[1024] = {0};
    
    struct HEART *heart = (struct HEART *) arg;
    //printf("heart ind=%d\n", heart->ins);
    while (1) {
        Node *p = NULL, *pre = NULL;
        for(int i = 0; i < heart->ins; i++) {
            p = heart->list[i];
            while(p && p->next) {
                pre = p;
                p = p->next;
                if(connect_sock_test(p->client_addr, heart->port_C)) {
                    printf("%s:%d not online\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                    pre->next = p->next;
                    free(p);
                    p = pre;
                    heart->sum[i]--;
                } else {
                    printf("%s:%d online\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
                }
            }
        }
        sleep(5);
    }
    return NULL;
}

struct sig_Node {
    LinkedList *list;
    int ins;
    int *sum;
    int sig_port;
};

void *sig(void *argv) {
    struct sig_Node *sig_node = (struct sig_Node *) argv;
    
    while(1) {
        sleep(20);
        for(int i = 0; i < sig_node->ins; i++) {
            Node *p = NULL;
            pthread_mutex_lock(&list_mutex);
            p = sig_node->list[i];
            while(p && p->next) {
                p = p->next;
                
                int connect_fd;
                char host[30] = {0};
                strcpy(host, inet_ntoa(p->client_addr.sin_addr));
                DBG("发送sig给%s:%d\n",host, sig_node->sig_port);
                if((connect_fd = socket_connect_nonblock(sig_node->sig_port, host)) < 0) {
                    DBG("发送失败\n");
                    continue;
                }
                int renum = 0;
                int flag = 0;
                for(int i = 100; i <= 105; i++) {
                    send(connect_fd, &i, sizeof(int), 0);
                    recv(connect_fd, &renum, sizeof(int), 0);
                    if(renum == (i + 300)) {
                        DBG("no %d\n", i);
                        flag = 1;
                    }
                }
                if(!flag) {
                    renum = 1;
                    send(connect_fd, &renum, sizeof(int), 0);
                }
                close(connect_fd);      
            }
            pthread_mutex_unlock(&list_mutex);
        }
    }
}

struct Recv {
    int file_port;
};

void *recv_file(void *argv) {
    struct Recv *recv_node = (struct Recv *) argv;
    int fd = socket_create(recv_node->file_port);
    if(fd < 0) {
        perror("recv file fd");
        return NULL;
    }
    int data;
    char buffer[1024] = {0};
    int socket_fd;
    while(1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        if((socket_fd = accept(fd, (struct sockaddr *) &addr, &len)) < 0) {
            perror("recv accept");
            continue;
        }
        char ip_log[30] = {0};
        strcpy(ip_log, inet_ntoa(addr.sin_addr));
        mkdir(ip_log, 0777);
        DBG("make dir :%s\n", ip_log);
        recv(socket_fd, buffer, sizeof(buffer), 0);
        char file_name[100];
        strcpy(file_name, buffer);
        DBG("接收文件 %s 开始\n", file_name);
        strcat(ip_log, "/");
        strcat(ip_log, file_name);
        FILE *fp = NULL;
        fp = fopen(ip_log, "a+");
        DBG("写文件到%s\n", ip_log);
        if(fp < 0) {
            perror("sever fopen");
            Write_Pi_Log(PiHealthLog, "server fopen:%s\n", strerror(errno));
            return NULL;
        }
        int data = 0;
        while ((data = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0) {
            //DBG("写入字符串:%s\n", buffer);
            DBG("%d ", data);
            fprintf(fp, "%s", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        DBG("\n");
        close(socket_fd);
        fclose(fp);
    } 
}

//sysinfo 104 warning
void *recv_warning(void *argv) {
    int w_port = *(int *)argv;
    while (1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int w_fd = socket_udp_server(w_port, addr);
        if(w_fd < 0) {
            perror("warning fd");
            Write_Pi_Log(PiHealthLog, "warning fd:%s\n", strerror(errno));
            return NULL;
        }
        char buffer[300] = {0};
        char msg[300] = {0};
        recvfrom(w_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, &len);
        DBG("warning!(id:%s)---%s\n", inet_ntoa(addr.sin_addr), buffer);
        Write_Pi_Log(PiHealthLog, "warning!(id=%s)--%s\n", inet_ntoa(addr.sin_addr), buffer);
        snprintf(msg, sizeof(msg), "INSERT INTO warning_events(wip,wtypes,wdetails) values('%s', 104, '%s')", inet_ntoa(addr.sin_addr), buffer);
        //DBG("%s\n", msg);
        MYSQL *ptr;
        ptr = connect_db(ptr, "localhost", "root", "hzc0000", "pihealth");
        if(insert_warning(msg, ptr)) {
            DBG("insert done!\n");
        }
        mysql_close(ptr);
    }
}


