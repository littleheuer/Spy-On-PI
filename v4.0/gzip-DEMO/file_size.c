/*************************************************************************
	> File Name: file_size.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月24日 星期日 16时43分25秒
 ************************************************************************/

#include<stdio.h>
#include<sys/stat.h>

int main() {
    struct stat st;
    char filename[] = "../client0316.c";
    stat(filename, &st);
    printf("size=%ld\n", st.st_size);
    return 0;
}
