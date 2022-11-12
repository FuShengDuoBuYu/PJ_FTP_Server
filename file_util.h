# ifndef FILE_UTIL_H
# define FILE_UTIL_H

#define MAX_FILE_SIZE 512
# include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>

// TODO: 统一命令和数据的传输格式
typedef struct {  
    int  file_tag;  //是否最后一片标志位
    int  file_rmd;  //最后一片文件字节长度
    int file_order; //文件分块的序号
    int enter_sspqce; //换行符的个数
    char buffer[MAX_FILE_SIZE+1];  //要发送的文件字节
}FileInfo;

int file_exists(const char *filename);
//获取文件的内容,并将内容存储到buffer中
int get_file_content(const char *filename, char *buffer,int buffer_index,int *enter_count);
//将buffer中的内容写入到文件中
int write_file_content(const char *filename, char *buffer);

int write_file_info(const char *filename, FileInfo *file_info);

// 获取当前工作目录
char* get_current_dir();

// 获取当前目录文件
char* get_current_ls();

// 修改当前目录
char* change_current_dir(char* relative_path);

// 创建新目录
int create_dir(char* dir_name);

// 删除指定文件
int delete_file(char* file_name);

// 获取每个文件块的数据 存储到FileInfo中
FileInfo* generate_file_info(const char *filename, char *buffer, int buffer_index,int* enter_count);
# endif