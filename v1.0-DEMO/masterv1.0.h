/*************************************************************************
	> File Name: master.h
	> Author: 
	> Mail: 
	> Created Time: Sat 09 Mar 2019 18:08:47 CST
 ************************************************************************/
#include "common.h"
#define TIME_OUT_TIME 100000

typedef struct Node{
    struct sockaddr_in client_addr;
    struct Node *next;
}Node, *LinkedList;

struct PRINT{
    LinkedList head;
    int index;
}; 

struct HEART{
    LinkedList *list;
    int ins;
    int *sum;
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
        printf("ip[%d] = %d\n",count, ip[count]);
        count++;
        if (*sip == '\0') {
            if (count == 4) return 0;
        } else {
            sip++;
        }
    }
    printf("\n");
    return 0;
}


int insert(LinkedList head, Node *node) {
    Node *p;
    p = head;
    while (p->next != NULL) {
       // printf("insert %s\n", inet_ntoa(p->next->client_addr.sin_addr));
        p = p->next;

    }
    p->next = node;
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


void *print(void *arg) {
    struct PRINT *print_para = (struct PRINT *)arg;
    int index = print_para->index;
    printf("index = %d, %d\n", index, print_para->index);
    char filename[50] = {0};
    sprintf(filename, "./%d.log", index);
    int temp = 0;
    while (1) {
        FILE *file = fopen(filename, "w");
        Node *p = print_para->head;
        fprintf(file, "%d\n", temp++);
        while (p -> next != NULL) {
            //printf("index=%d,%s:%d\n", index, inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
            p = p->next;
            fprintf(file, "%s:%d\n", inet_ntoa(p->client_addr.sin_addr), ntohs(p->client_addr.sin_port));
        }
        fclose(file);
        sleep(1);
    }

    return NULL;
}

//查重
int check(LinkedList *head, struct sockaddr_in addr, int ins) {
    for(int i = 0; i < ins; i++) {
        Node *p =head[i];
        while(p->next) {
            if(addr.sin_addr.s_addr == p->next->client_addr.sin_addr.s_addr) {
                DBG("%s\n **in list** ", inet_ntoa(addr.sin_addr.s_addr));
                return 1;
            }
        }
    }
    return 0;
}

int connect_sock_test_bak(struct sockaddr_in addr) {
    int fd;
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket connetc test");
        return -1;
    }
    if(connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(fd);
        return 1;
    }
    close(fd);
    printf("connect test\n");
    return 0;
}

int connect_sock_test(struct sockaddr_in addr) {
    int fd;
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket connetc test");
        return -1;
    }
    
    int error = -1, len = sizeof(int);
    struct timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(fd, FIONBIO, &ul);
    bool ret = false;

    if(connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        tm.tv_sec = 0;
        tm.tv_usec = TIME_OUT_TIME;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if(select(fd + 1, NULL, &set, NULL, &tm) > 0) {
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if(0 == error) ret = true;
            else ret = false;
        } else ret = false;
    } else ret = false;
    ul = 0;
    ioctl(fd, FIONBIO, &ul);
    if(!ret) {
        close(fd);
        //fprintf(stderr, "Cannot connect the client\n");
        return 1;
    }
    close(fd);
    printf("connect test\n");
    return 0;
}
void *heart_beat(void *arg) {
    struct HEART *heart = (struct HEART *) arg;
    printf("heart ind=%d\n", heart->ins);
    while (1) {
        Node *p = NULL, *pre = NULL;
        for(int i = 0; i < heart->ins; i++) {
            p = heart->list[i];
            while(p && p->next) {
                pre = p;
                p = p->next;
                if(connect_sock_test(p->client_addr)) {
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
