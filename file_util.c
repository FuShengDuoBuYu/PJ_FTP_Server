# include "file_util.h"


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
    int len = fread(buffer, 1, MAX_FILE_SIZE, fp);
    while(i<len){
        if (buffer[i] == EOF){
            break;
        }
        //fseek会将换行看作两个字符,所以要将换行的次数记录下来,下次从加上换行的次数开始读取
        if(buffer[i] == '\n'){
            (*enter_count)++;
        }
        i++;
    }
    fclose(fp);
    return i;
}

int get_file_content_binary(const char *filename, char *buffer,int buffer_index,int *enter_count){
    FILE *fp = fopen(filename, "rb");
    if(fp == NULL){
        return 0;
    }
    int i = 0;
    //偏移到buffer_index * MAX_FILE_SIZE的位置
    fseek(fp, buffer_index * MAX_FILE_SIZE+(*enter_count), SEEK_SET);
    int len = fread(buffer, 1, MAX_FILE_SIZE, fp);
    fclose(fp);
    return len;
}

void generate_file_info(const char *filename, FileInfo *fileInfo, int buffer_index, int *enter_count, FileType fileType){
    // FileInfo* file_info = (FileInfo*)malloc(sizeof(FileInfo));
    fileInfo->file_tag = 0;
    fileInfo->file_rmd = 0;
    fileInfo->file_order = buffer_index;
    memset(fileInfo->buffer, 0, sizeof(fileInfo->buffer));
    //获取文件的内容,每次取1024字节,然后发送,直到最后一个发送的不到1024,代表文件已经发送完毕
    int last_send_size = MAX_FILE_SIZE;
    last_send_size = fileType == BINARY_FILE ? get_file_content_binary(filename, fileInfo->buffer, buffer_index, enter_count) : get_file_content(filename, fileInfo->buffer, buffer_index, enter_count);
    if(last_send_size < MAX_FILE_SIZE){
        fileInfo->file_tag = 1;
    }
    fileInfo->file_rmd = last_send_size;
}

int write_file_info(char *filename, FileInfo *file_info, FileType fileType){
    FILE *fp = fileType == BINARY_FILE ? fopen(filename, "ab+") : fopen(filename, "a+");
    if(fp == NULL){
        return 0;
    }
    fwrite(file_info->buffer, 1, file_info->file_rmd, fp);
    fclose(fp);
    return 1;
}

// get current working directory
char* get_current_dir(){
    char* current_dir = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
    memset(current_dir, 0, sizeof(current_dir));
    getcwd(current_dir, MAX_FILE_SIZE);
    return current_dir;
}

// get current dir file list (like linux ls command)
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

char* change_current_dir(char* relative_path){
    char* current_dir = get_current_dir();
    strcat(current_dir, "\\");
    strcat(current_dir, relative_path);
    // printf("%d\n", chdir(current_dir));
    if(chdir(current_dir) == 0){
        return get_current_dir();
    } else {
        return NULL;
    }
}

// 创建新文件夹
int create_dir(char* dir_name){
    char* current_dir = get_current_dir();
    strcat(current_dir, "\\");
    strcat(current_dir, dir_name);
    if(mkdir(current_dir) == 0){
        return 1;
    } else {
        return 0;
    }
}

// 删除文件
int delete_file(char* filename){
    char* current_dir = get_current_dir();
    strcat(current_dir, "\\");
    strcat(current_dir, filename);
    if(remove(current_dir) == 0){
        return 1;
    } else {
        return 0;
    }
}