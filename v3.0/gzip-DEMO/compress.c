/*************************************************************************
	> File Name: compress.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月24日 星期日 18时58分37秒
 ************************************************************************/

#include<stdio.h>
#include<zlib.h>
#include<stdlib.h>

#define MaxBufferSize 1024 * 10

int main () {
    FILE * file_src, *file_tmp;
    unsigned long len_src, len_tmp;
    unsigned char *buffer_src = (unsigned char *)malloc(sizeof(unsigned char *) * MaxBufferSize);
    unsigned char *buffer_tmp = (unsigned char *)malloc(sizeof(unsigned char *) * MaxBufferSize);
    file_src = fopen("te.txt", "r");
    len_src = fread(buffer_src, sizeof(char), MaxBufferSize - 1, file_src);
    compress(buffer_tmp, &len_tmp, buffer_src, len_src);
    file_tmp = fopen("compress.txt", "w");
    
    fwrite(buffer_tmp, sizeof(char), len_tmp, file_tmp);
    fclose(file_tmp);
    return 0;
}
