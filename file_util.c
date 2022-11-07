# include "file_util.h"
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>

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
// TODO: dynamic dir
char* get_current_dir(){
    char* current_dir = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
    memset(current_dir, 0, sizeof(current_dir));
    getcwd(current_dir, MAX_FILE_SIZE);
    return current_dir;
}

// get current dir file list (like linux ls command)
// TODO: dynamic dir
char* get_current_ls(){
    long handle;  //用于查找的句柄
    struct _finddata_t fileinfo; //文件信息的结构体

    char* current_dir = get_current_dir();

	strcat(current_dir, "\\*");
    
    char* file_list = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
    memset(file_list, 0, sizeof(file_list));

    handle=_findfirst(current_dir,&fileinfo); //第一次查找
    if(-1==handle) 
        return NULL;
    // printf("%s\n",fileinfo.name); //打印出找到的文件的文件名

    int i = 1;
    strcat(file_list, fileinfo.name);
    strcat(file_list, "\t\t");
	while(!_findnext(handle,&fileinfo)) //循环查找其他符合的文件，知道找不到其他的为止
	{
        i++;
        strcat(file_list, fileinfo.name);
        if(i%3 == 0){
            strcat(file_list, "\n");
        } else {
            strcat(file_list, "\t\t");
        }
		// printf("%s\n",fileinfo.name);
	}
	_findclose(handle); //别忘了关闭句柄
    
    return file_list;
}