# include "file_util.h"
#include <stdlib.h>
#include <string.h>
#include <direct.h>

int file_exists(const char *filename){
    printf("file %s\n", filename);
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        return 0;
    }
    fclose(fp);
    return 1;
}

int get_file_content(const char *filename, char *buffer,int buffer_index,int *enter_count){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        return 0;
    }
    int i = 0;
    //偏移到buffer_index * MAX_FILE_SIZE的位置
    fseek(fp, buffer_index * MAX_FILE_SIZE+(*enter_count), SEEK_SET);
    while(!feof(fp)&&i<MAX_FILE_SIZE){
        buffer[i] = fgetc(fp);
    //fseek会将换行看作两个字符,所以要将换行的次数记录下来,下次从加上换行的次数开始读取
        if(buffer[i] == '\n'){
            (*enter_count)++;
        }
        i++;
    }
    fclose(fp);
    return i;
}

int write_file_content(const char *filename, char *buffer){
    FILE *fp = fopen(filename, "a");
    if(fp == NULL){
        return 0;
    }
    int i = 0;
    while(buffer[i] != '\0'){
        fputc(buffer[i], fp);
        i++;
    }
    fclose(fp);
    return 1;
}

// get current working directory
char* get_current_dir(){
    char* current_dir = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
    memset(current_dir, 0, sizeof(current_dir));
    getcwd(current_dir, 1024);
    return current_dir;
}