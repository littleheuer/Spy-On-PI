/*************************************************************************
	> File Name: epoll.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月04日 星期四 19时16分35秒
 ************************************************************************/

#include<stdio.h>
#include<sys/epoll.h>
#include<sys/socket.h>

int main () {
    int sock_fd, port;
    struct sockaddr_in addr;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&s_addr, sizesof(s_addr));
    addr.sin_family = AF_INET;
    addr.sin_pot = 7731;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    if(bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return -1;
    }
    if(listen(sock_fd, 300) < 0) {
        perror("listen");
        close(sock_fd);
        return -1;
    }

    int epollfd;
    epollfd = epoll_create1(0);
    struct epoll_event ev, event[100];
    ev.events = EPOLLIN;
    ev.data.fd = sock_fd;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &ev);
    
    while (1) {
        int nfds = epoll_wait(epollfd, event, MAX_EVENTS, -1);
        for(int i = 0; i < nfds; ++i) {
            if(event[i].data.fd == sock_fd && (event[i].events & EPOLLIN)) {
                int acc_fd = accept(sock_fd, NULL, NULL);
                if(acc_fd == -1) {
                    perror("acc fd");
                    return -1;
                }
                //连接成功的机器fd为可读
                ev.data.fd. = sockfd;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, acc_fd, &ev);
            } else if(event[i].events & EPOLLIN) {
                char buff[400] = {0};
                int recv_data = recv(event[i].data.fd, buff, sizeof(buff), 0);
                if(recv_data <= 0) {
                    close(sockfd);
                    event[i].data.fd = -1;
                    continue;
                } else {
                    printf("%s\n", buff);
                }
                ev.events = EPOLLOUT;
                //ev.data.ptr =  
                ev.data.fd = event[i].data.fd;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, event[i].data.fd, &ev)
            }
        } else if(event[i].events & EPOLLOUT) {
            send(event[i].data.fd, "you qre here", 14, 0);
            close(event[i].data.fd);
            epoll_ctl(epollfd, EPOLL_CTL_DEL, event[i].data.fd, &ev);
        }
    }
    return 0;
}
