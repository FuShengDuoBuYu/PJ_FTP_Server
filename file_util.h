# ifndef FILE_UTIL_H
# define FILE_UTIL_H

#define MAX_FILE_SIZE 512
# include<stdio.h>
int file_exists(const char *filename);
//获取文件的内容,并将内容存储到buffer中
int get_file_content(const char *filename, char *buffer,int buffer_index,int *enter_count);
//将buffer中的内容写入到文件中
int write_file_content(const char *filename, char *buffer);

// 获取当前工作目录
char* get_current_dir();

// 获取当前目录文件
char* get_current_ls();

// 修改当前目录
char* change_current_dir(char* relative_path);

// 创建新目录
int create_dir(char* dir_name);
# endif