/*************************************************************************
	> File Name: master.c
	> Author: 
	> Mail: 
	> Created Time: Sat 09 Mar 2019 18:09:47 CST
 ************************************************************************/

#include "master.h"

int main() {
    char config[] = "../default.conf";
    int start[4];
    int finish[4];
    char start_ip[20] = {0};
    char finish_ip[20] = {0};
    char port_c[6] = {0}; 
    char port_m[6] = {0};
    char temp[6] = {0};
    char ins_s[5] = {0};
    int ins = 0, port_C, port_M, sig_port, file_port, w_port;
    
    int tmp = fork();
    if(tmp) return 0;
    pthread_t print_t;
    pthread_mutex_t mutex;
    //INS 线程数
    //From(start)~To(finish) 带插入的测试ip,
    //client_port 客户端端口
    //master_port 服务端监听端口
    get_conf_value(config, "INS", ins_s);
    get_conf_value(config, "From", start_ip);
    get_conf_value(config, "To", finish_ip);
    get_conf_value(config, "client_port", port_c);
    get_conf_value(config, "master_port", port_m);
    
    get_conf_value(config, "file_port", temp);
    file_port = atoi(temp);
    get_conf_value(config, "sig_port", temp);
    sig_port = atoi(temp);
    get_conf_value(config, "w_port", temp);
    w_port = atoi(temp);
    
    ins = atoi(ins_s);
    port_C = atoi(port_c);
    port_M = atoi(port_m);
    transip(start_ip, start);
    transip(finish_ip, finish);
    //记录每个线程的链表的节点数
    int *sum = (int *)malloc(ins * sizeof(int));

    memset(sum, 0, ins *sizeof(int));

    DBG("start = %d.%d.%d.%d\n", start[0], start[1], start[2], start[3]);
    DBG("finish = %d.%d.%d.%d\n", finish[0], finish[1], finish[2], finish[3]);
    //存每个线程的链表表头
    LinkedList *linkedlist = (LinkedList *)malloc(ins * sizeof(LinkedList));
    //初始化的addr
    struct sockaddr_in initaddr;
    initaddr.sin_family = AF_INET;
    initaddr.sin_port = htons(port_C);
    initaddr.sin_addr.s_addr = inet_addr("0.12.0.0");
    //初始化每个链表表头并存在linkedlist中
    for (int i = 0; i < ins; i++) {
        Node *p;
        p = (Node *)malloc(sizeof(Node));
        p->client_addr = initaddr;
        p->next = NULL;
        linkedlist[i] = p;
    }
    
    DBG("%s\n", inet_ntoa(linkedlist[0]->client_addr.sin_addr));
    //插入测试ip
    char host[20] = {0};

    for (int i = start[3]; i <= finish[3]; i++) {
        
        sprintf(host, "%d.%d.%d.%d", start[0], start[1], start[2], i);
        initaddr.sin_addr.s_addr = inet_addr(host);
        Node *p;
        p = (Node *)malloc(sizeof(Node));
        p->client_addr = initaddr;
        p->next = NULL;
        //查找最短的链表并插入其中
        int sub = find_min(sum, ins);
         
        insert(linkedlist[sub], p);
        sum[sub]++;
    }

    fflush(stdout);

    //print_para为传入回调函数的参数:包括链表编号和表头
    //t[ins]存每个线程序列号
    struct PRINT print_para[ins];
    pthread_t t[ins];
    for (int i = 0; i < ins; i++) {
        print_para[i].index = i;
        print_para[i].head = linkedlist[i];
        if (pthread_create(&t[i], NULL, print, (void *)&print_para[i]) == -1) {
            DBG("error in pthread_create!\n");
            return -1;
        }
    }
    
    //心跳检测
    struct HEART heart;
    heart.list = linkedlist;
    heart.ins = ins;
    heart.sum = sum;
    heart.port_C = port_C;
    pthread_t heart_t;
    if(pthread_create(&heart_t, NULL, heart_beat, (void *) &heart) < 0) {
        DBG("error in heart pthread create\n");
        Write_Pi_Log(PiHealthLog, "heart pthread:%s\n", strerror(errno));
        return -1;
    }

    
    //互发信号
    pthread_t sig_t;
    struct sig_Node sig_node;
    sig_node.list = linkedlist;
    sig_node.ins = ins;
    sig_node.sum = sum;
    sig_node.sig_port = sig_port;
    pthread_create (&sig_t, NULL, sig, (void *)&sig_node);
    
    //接收文件
    pthread_t recv_t;

    struct Recv recv;
    recv.file_port = file_port;
    pthread_create (&recv_t, NULL, recv_file, (void *)&recv);
    
    //回收warning信息并写入数据库中
    pthread_t warning_t;
    pthread_create(&warning_t, NULL, recv_warning, (void *) &w_port);
    
    //创建套接字监听端口
    int server_listen;
    if((server_listen = socket_create(port_M)) < 0) {
        DBG("%s\n", strerror(errno));
        Write_Pi_Log(PiHealthLog, "server listen:%s\n", strerror(errno));
        return 1;
    }

    DBG("开始监听端口:%d\n", port_C);
    //do_epoll(server_listen, sum, ins, linkedlist, port_C);
    do_epoll_1(server_listen, sum, ins, linkedlist, port_C);
    /*****
    while (1) {
        //接收客户端的addr结构体
        struct sockaddr_in client_addr;
        int fd;
        socklen_t len = sizeof(client_addr);
        if((fd = accept(server_listen, (struct sockaddr *)&client_addr, &len)) < 0) {
            DBG("%s\n", strerror(errno));
            Write_Pi_Log(PiHealthLog, "listen accept::%s\n", strerror(errno));
            close(fd);
            continue;
        }

        DBG("插入链表-->%s\n", inet_ntoa(client_addr.sin_addr));

        Node *p;
        p = (Node *)malloc(sizeof(Node));
        client_addr.sin_port = htons(port_C);
        p->client_addr = client_addr;
        p->next = NULL;
        int sub = find_min(sum, ins);
        //查找ip是否已存在,遍历每条线程的链表
        if(check(linkedlist, client_addr, ins)) {
            continue;
        }
        //插入到链表中
        insert(linkedlist[sub], p);
    }      
    *****/
    
    for (int i = 0; i < ins; i++) {
        pthread_join(t[i], NULL);
    }

    pthread_join(recv_t, NULL);
    pthread_join(warning_t, NULL);
    pthread_join(heart_t, NULL);
    pthread_join(print_t, NULL);
    return 0;
}

