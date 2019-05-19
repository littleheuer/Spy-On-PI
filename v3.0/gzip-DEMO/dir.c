/*************************************************************************
	> File Name: dir.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月30日 星期六 10时53分17秒
 ************************************************************************/

#include<stdio.h>
#include"../common/common.h"
#include<dirent.h>

int main() {
    struct dirent *dir_con;
    char dirname[] = "../client" ;
    DIR *dir_fp = opendir(dirname);
    int i = 1;
    while (dir_con = readdir(dir_fp)) {
        printf("Filename %d = %s\n", i, dir_con->d_name);
        printf("file type = %d\n", dir_con->d_type);
    }
    closedir(dir_fp);
    return 0;


}
