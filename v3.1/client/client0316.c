/*************************************************************************
	> File Name: client0316.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月16日 星期六 14时09分39秒
 ************************************************************************/

#include<stdio.h>
#include<zlib.h>
#include "common.h"
#define MAX_TIME 10

struct sm_msg {
    int flag;//检测次数
    int time;//延迟检测心跳时间
    //要在进程里使用线程锁，需要初始化一下属性
    pthread_mutex_t sm_mutex;
    pthread_cond_t sm_ready;
};

char config[] = "../default.conf";
char PiHealthLog[] = "./PiHealth.log";
char *share_memory = NULL;
struct sm_msg *msg;
pthread_mutexattr_t m_attr;
pthread_condattr_t c_attr;
int dyaver = 20;

int client_port, master_port, file_port, sig_port, w_port;
char port_temp[6] = {0}, master_ip[20];

void send_warning(char *buff) {
    if(NULL != strstr(buff, "warning")) {
        struct sockaddr_in addr;
        int w_fd = socket_udp_client(w_port, addr, master_ip);
        if(w_fd < 0) {
            perror("warning fd udp");
            close(w_fd);
            return ;
        }
        printf("发送警告信息:%s\n", buff);
        sendto(w_fd, buff, strlen(buff), 0, (struct sockaddr *) &addr, sizeof(addr));
        close(w_fd);
    }
    return ;
}

//获取文件大小(/bytes)
long long file_size(char *filename) {
    struct stat statbuf;
    stat(filename, &statbuf);
    long long size = statbuf.st_size;
    
    return size;
}

int find_file(char *dir_name, char *filename, const char *ch) {
    struct dirent *dir_con;
    DIR *dir_fp = opendir(dir_name);
    char *result = NULL;
    while(dir_con = readdir(dir_fp)) {
        result = strstr(dir_con->d_name, ch);
        if(result != NULL) {
            strcpy(filename, dir_con->d_name);
            closedir(dir_fp);
            return 1;
        }
    }
    closedir(dir_fp);
    return 0;
}

//压缩
int zip_f(char *filename, int n) {
    gzFile file;
    char buffer[1024] = {0};
    char dest_name[10] = {0};
    sprintf(dest_name, "%s-%d.gz", filename, n);
    file = gzopen(dest_name, "wb");
    if(NULL == file) {
        perror("can't open gzfile");
        return -1;
    }
    //?
    gzsetparams(file, 2, 0);
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    if(fp == NULL) {
        perror("zip fopen");
        return -1;
    }
    if(flock(fp->_fileno, LOCK_SH) < 0) {
        Write_Pi_Log(PiHealthLog, "zip fp fclock:%s\n", strerror(errno));
    }
    while (fgets(buffer, 1024, fp)) {
        int len = strlen(buffer);
        gzwrite(file, buffer, len);
    }
    gzclose(file);
    fclose(fp);
    return 0;
}

//执行脚本，写入文件中
void sys_detect(int type) {
    
    int n = 0;
    int time_i = 0;
    char temp_src[50] = {0};
    char src[50] = {0};
    sprintf(temp_src , "src%d", type);
    char run[100] = {0};
    char buffer[8192] = {0};
    
    get_conf_value(config, temp_src, src);
    
    FILE *fstream = NULL;
    int times = 0;
    char temp[4] = {0};
    char logname[50] = {0};
    get_conf_value(config, "write_every_times", temp);
    times = atoi(temp);
    char log_dir[40] = {0};
    get_conf_value(config, "log_dir", log_dir);

    switch(type) {
        case 100 ://cpu 
            time_i = 5; 
            sprintf(logname, "%s/cpu.log", log_dir);    
            break;
        case 101 ://mem
            time_i = 5; 
            
            sprintf(logname, "%s/mem.log", log_dir);    
            break;
        case 102 ://disk
            time_i = 60;
            sprintf(logname, "%s/disk.log", log_dir);
            break;
        case 103 ://pro
            time_i = 30;
            sprintf(logname, "%s/ps.log", log_dir);
            break;
        case 104 ://sysinfo
            time_i = 60;
            sprintf(logname, "%s/sysinfo.log", log_dir); 
            break;
        case 105 ://userinfo
            time_i = 60;
            sprintf(logname, "%s/userinfo.log", log_dir);
            break;
    }
    sprintf(run, "bash %s", src);
    if(type == 101) sprintf(run, "bash %s %d", src, dyaver);

    while(1) {
        fstream = NULL;
        for(int i = 0; i < times; i++) {
            char buff[400] = {0};
            if(NULL == (fstream = popen(run, "r"))) {
                printf("popen error\n");
                Write_Pi_Log(PiHealthLog, "popen%s\n", strerror(errno));
                return ;
            }
            if(type == 101) {
                if(NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                    send_warning(buff);
                }
                if(NULL != fgets(buff, sizeof(buff), fstream)) {
                    dyaver = atof(buff);
                } else {
                    dyaver = 0;
                }
            } else {
                while(NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                    send_warning(buff);
                }
            }

            //自检超过MAX_TIME次就开启心跳检测
            if(type == 100) {
                //printf("\033[31m$\033[0m ");
                fflush(stdout);
                pthread_mutex_lock(&msg->sm_mutex);
                if(++msg->flag >= MAX_TIME) {
                    if(msg->time == 0) {
                        Write_Pi_Log(PiHealthLog, "\n自检超过\033[33m%d\033[0m次,master未心跳\n", msg->flag);
                        pthread_cond_signal(&msg->sm_ready);
                        Write_Pi_Log(PiHealthLog, "发送信号ready,start heart beat!\n");
                    }
                    msg->flag = 0;    
                }
                pthread_mutex_unlock(&msg->sm_mutex);
            }
        }
        //将buffer写入文件中
        FILE *fd = fopen(logname, "a+");
        if(NULL == fd) {
            Write_Pi_Log(PiHealthLog, "error open logfile %s\n", logname);
            exit(1);
        }
        
        if(flock(fd->_fileno, LOCK_EX) < 0) {
            Write_Pi_Log(PiHealthLog, "flock:%s\n", strerror(errno));
        }

        fwrite(buffer, 1, strlen(buffer), fd);
        fclose(fd);
        memset(buffer, 0, sizeof(buffer));
        if(file_size(logname) > 1024 * 1024 * 30) {
            if(!zip_f(logname, n)) {
                printf("压缩文件done\n");
                Write_Pi_Log(PiHealthLog, "压缩文件done\n");
                remove(logname);
                n++;
            }
        }
        sleep(time_i);
    }
    
}

bool client_heart(char *ip, int port) {
    int fd = socket_connect(port, ip);
    if(fd < 0) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}


int send_file(int file_port, char *master_ip) {
    char buffer[1024] = {0};
    char logtemp[10] = {0};
    char logname[40] = {0};//配置文件的映射文件名
    char zipname[40] = {0};//若有zip文件，获取文件名
    char dir[40] = {0};//log路径
    char dirname[40] = {0};//带路径的文件名
    
    for(int i = 200; i <=205; i++) {
        int file_fd = socket_connect(file_port, master_ip);
        if(file_fd < 0) {
            perror("send file fd");
            close(file_fd);
            return -1;
        }
        //src代称
        snprintf(logtemp, sizeof(logtemp), "src%d", i);    
        get_conf_value(config, logtemp, logname);
    
        DBG("发送文件名:%s\n", logname);
        send(file_fd, logname, sizeof(logname), 0);    
        //文件路径
        get_conf_value(config, "log_dir", dir);
        sprintf(dirname, "%s/%s", dir, logname);
        
        while(find_file(dir, zipname, logname)) {
            if(strstr(zipname, ".gz") != NULL) {
                gzFile file;
                sprintf(dirname, "%s/%s", dir, zipname);
                DBG("读取备份文件(dir=%s)\n", dirname);
                if((file = gzopen(dirname, "rb")) == 0) {
                    Write_Pi_Log(PiHealthLog, "gzopen error\n");
                    DBG("gzopen error:%s\n", strerror(errno));
                    return -1;
                }
                int real = 0;
                while((real = gzread(file, buffer, sizeof(buffer) - 1)) > 0) {
                    buffer[real] = '\0';
                    //DBG("发送字符串:%s\n", buffer);
                    DBG("%d ", real); 
                    send(file_fd, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                }
                DBG("\n");
                gzclose(file);
                if(!remove(dirname)) {
                    Write_Pi_Log(PiHealthLog, "delete %s ok!\n", dirname);
                    DBG("delete %s ok!\n", dirname);
                    memset(zipname, 0, sizeof(zipname));
                }
            } else if(strstr(zipname, ".gz") == NULL){
                FILE *fp = NULL;
                if((fp = fopen(dirname, "r")) == NULL) {
                    Write_Pi_Log(PiHealthLog, "fopen error\n");
                    DBG("fopen error:%s\n", strerror(errno));
                    DBG("file name:%s\n", dirname);
                    return -1;
                }
                //共享锁
                if(flock(fp->_fileno, LOCK_SH) < 0) {
                    Write_Pi_Log(PiHealthLog, "flock LOCK_SH failed:%s\n", strerror(errno));
                }

                DBG("开始读取未压缩文件:%s\n", dirname);
                
                while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
                    //DBG("发送字符串:%s\n", buffer);
                    int data = strlen(buffer);
                    //DBG("%d ", data);
                    send(file_fd, buffer, data, 0);
                    //fflush(stdout);
                }
                //DBG("\n");
                fclose(fp);
                if(!remove(dirname)) {
                    Write_Pi_Log(PiHealthLog, "delete %s ok!\n", logname);
                    DBG("delete %s ok!\n", dirname);
                    memset(zipname, 0, sizeof(zipname));
                }
            }
        }
        close(file_fd);
    }
    return 0;
}

int main() {
    //定义共享内存的地址
    int shmid;
    int heart_listen_fd;
    
    get_conf_value(config, "client_port", port_temp);
    client_port = atoi(port_temp);
    get_conf_value(config, "master_port", port_temp);
    master_port = atoi(port_temp);
    get_conf_value(config, "master_ip", master_ip);
    
    get_conf_value(config, "file_port", port_temp);
    file_port = atoi(port_temp); 
    get_conf_value(config, "sig_port", port_temp);
    sig_port = atoi(port_temp);  
    get_conf_value(config, "w_port", port_temp);
    w_port = atoi(port_temp);  


    if((shmid = shmget(IPC_PRIVATE, sizeof(struct sm_msg), 0666|IPC_CREAT)) == -1) {
        Write_Pi_Log(PiHealthLog, "Error in shmget : %s\n", strerror(errno));
        DBG("Error in shmget : %s\n", strerror(errno));
        return -1;
    }
    //shmat(shmid, NULL, flags), flag 指定此内存段是否为可读
    //函数返回指向此段的指针
    share_memory = (char *) shmat(shmid, 0, 0);
    if(share_memory == NULL) {
        Write_Pi_Log(PiHealthLog, "shmat : %s\n", strerror(errno));
        DBG("shmat : %s\n", strerror(errno));
        return -1;
    }
    //共享内存指针赋值给msg使用
    msg = (struct sm_msg *)share_memory;
    msg->flag = 0;
    msg->time = 0;
    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&msg->sm_mutex, &m_attr);
    pthread_cond_init(&msg->sm_ready, &c_attr);

    int pid;
    if((pid = fork()) < 0) {
        Write_Pi_Log(PiHealthLog, "fork error\n");
        DBG("fork error\n");
        return -1;
    }
    
    //主进程等待master对自己进行心跳检测
    if(pid != 0) {
        //主进程
        if((heart_listen_fd = socket_create(client_port)) < 0) {
            Write_Pi_Log(PiHealthLog, "Error in sock_create\n");
            DBG("Error in sock_create\n");
            return -1;
        }
        DBG("开始10000端口心跳监听!\n");
        while (1) {
            int fd;
            if((fd = accept(heart_listen_fd, NULL, NULL)) < 0) {
                close(fd);
                Write_Pi_Log(PiHealthLog, "accept error on heart\n");
                DBG("accept error on heart\n");
                continue;
            }
            pthread_mutex_lock(&msg->sm_mutex);
            msg->time = 0;
            msg->flag = 0;
            //DBG("\033[32m❤\033[0m");
            pthread_mutex_unlock(&msg->sm_mutex);
            char buf[20] = {0};
            while(recv(fd, buf, sizeof(buf), 0) <= 0) {
                close(fd);
                //DBG("\033[34m❤\033[0m");
            }
        }
    } else {
        //子进程
        //客户端上线即连接master
        int cfd;
        if((cfd = socket_connect_nonblock(master_port, master_ip)) < 0) {
            Write_Pi_Log(PiHealthLog, "cfd:%s\n", strerror(errno));
            DBG("cfd:%s\n", strerror(errno));
        } else {
            char buffer[100] = {0};
            
            //通过pwd获得本机主机名
            struct passwd *pwd;
            pwd = getpwuid(getuid());
            
            //获取对端主机名
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof(addr);
            if(getpeername(cfd, (struct sockaddr *)&addr, &addr_len)) {
                DBG("getpeername:%s\n", strerror(h_errno));
                return -1;
            }
            struct hostent *hp = NULL;
            if((hp = gethostbyaddr((void *)&addr.sin_addr, addr_len, AF_INET)) == NULL) {
                DBG("gethostbyaddr:%s\n", strerror(h_errno));
                return -1;
            }
            

            sprintf(buffer, "login->%s on %s\n", pwd->pw_name, hp->h_name);
            send(cfd, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(cfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
            close(cfd);
        }
        

        int pid_son;
        if((pid_son = fork()) < 0) {
            Write_Pi_Log(PiHealthLog, "pid son:%s\n", strerror(errno));
            DBG("pid son:%s\n", strerror(errno));
            return -1;
        }
        //第一个孙子进程死循环进行心跳检测
        if(pid_son == 0) {
            while (1) {
                pthread_mutex_lock(&msg->sm_mutex);
                //等待信号开启心跳检测
                pthread_cond_wait(&msg->sm_ready, &msg->sm_mutex);
                DBG("获得心跳许可，开始心跳!\n");
                
                while (1) {
                   
                    if(client_heart(master_ip, master_port)) {
                        DBG("\033[32m❤ √ \033[0m\n");
                        msg->flag = 0;
                        msg->time = 0;
                        fflush(stdout);
                        break;
                    } else {
                        DBG("\033[33m❤ × \033[0m");
                        msg->time++;
                        fflush(stdout);
                    }
                    sleep(20 * msg->time);
                    if(msg->time > MAX_TIME) msg->time = MAX_TIME;
                }
                pthread_mutex_unlock(&msg->sm_mutex);
            }
            
        } else {
            //子进程
            int sig_pid;
            if((sig_pid = fork()) < 0) {
                Write_Pi_Log(PiHealthLog, "sig fork:%s\n", strerror(errno));
                DBG("sig fork:%s\n", strerror(errno));
                return -1;
            }
            
            //第二个孙子进程
            //sig信号监听
            if(sig_pid == 0) {
                int sig_fd;
                if((sig_fd = socket_create(sig_port)) < 0) {
                    
                    Write_Pi_Log(PiHealthLog, "sig fd:%s\n", strerror(errno));
                    DBG("sig fd:%s\n", strerror(errno));
                    close(sig_fd);
                    return -1;
                }
                DBG("开启sig监听:%d\n", sig_port);
                int flag_sig;
                while (1) {
                    flag_sig = 0;
                    struct sockaddr_in master_addr;
                    int fd;
                    socklen_t len = sizeof(master_addr);
                    if((fd = accept(sig_fd, (struct sockaddr*) &master_addr, &len)) < 0) {
                        Write_Pi_Log(PiHealthLog, "sig:%s\n", strerror(errno));
                        DBG("sig:%s\n", strerror(errno));
                        close(fd);
                        continue;
                    }
                    int reallen = 0, num;
                    
                    pthread_mutex_lock(&msg->sm_mutex);
                    msg->time = 0;
                    msg->flag = 0;
                    DBG("\033[37;46m(づ￣3￣)\033[0m\n");
                    pthread_mutex_unlock(&msg->sm_mutex);
                    
                    while((reallen = recv(fd, &num, sizeof(int), 0)) > 0) {
                        if(num == 1) {
                            flag_sig = 1;
                            break;
                        }
                        //DBG("sig recv %d\n", num);
                        char log_dir[40] = {0};
                        get_conf_value(config, "log_dir", log_dir);
                        char src_name[40] = {0}, temp_s[40] = {0};
                        sprintf(temp_s, "src%d", num + 100);
                        get_conf_value(config, temp_s, src_name);
                        char zip_name[40] = {0};
                        int check = 0;
                        check = find_file(log_dir, zip_name, src_name);
                        if(!check){
                            num += 300;
                            send(fd, &num, sizeof(int), 0);
                            DBG("无%s ", src_name);
                        } else {
                            num += 100;
                            send(fd, &num, sizeof(int), 0);
                            DBG("有%s ", src_name);
                        }
                        DBG("\n");
                    }
                    close(fd);
                    if(flag_sig) send_file(file_port, master_ip);
                }
                close(sig_fd);

            } else if(sig_pid != 0){
                //子进程
                //主进程开6个进程分别执行脚本
                int i;
                int pid_grand;
                for(i = 100; i < 106; i++) {
                    if((pid_grand = fork()) < 0) {
                        perror("error fork pid_grand");
                        Write_Pi_Log(PiHealthLog, "fork pid grand:%s\n", strerror(errno));
                        DBG("fork pid grand:%s\n", strerror(errno));
                        continue;
                    }
                    if(pid_grand == 0) break;
                }
                if(pid_grand == 0) {
                    sys_detect(i);
                }
            }
        }
    }
    return 0;
}


