/*************************************************************************
	> File Name: data_link.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月23日 星期六 16时34分37秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <mysql/mysql.h>

MYSQL* connect_db(MYSQL *ptr, const char *host, const char *user, const char *pwd, const char *db) {
    ptr = mysql_init(NULL);
    if(ptr == NULL) {
        return NULL;
    }
    ptr = mysql_real_connect(ptr, host, user, pwd, db, 0, NULL, 0);
    if(ptr == NULL) {
        printf("connect error:%d,%s\n", mysql_errno(ptr), mysql_error(ptr));
        return NULL;
    }
    printf("connect success\n");
    return ptr;
}

int insert_warning(char *buffer, MYSQL *ptr) {
    int rs = 0;
    rs = mysql_real_query(ptr, buffer, strlen(buffer));
    if(0 != rs) {
        printf("mysql_real_query():%s\n", mysql_error(ptr));
        return 0;
    }
    return 1;
}


int main () {
    MYSQL *conn;
    conn = connect_db(conn, "localhost", "root", "hzc0000","test_1");
    char buffer[1024] = {0};
    sprintf(buffer, "INSERT INTO employee values(2, 'Mama', 100000.0);");
    insert_warning(buffer, conn);
    mysql_close(conn);
    return 0;
}
