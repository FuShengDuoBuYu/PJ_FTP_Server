#include "ftp_server.h"

void ftp_put(char* filename,SOCKET sclient){
    FileInfo file_info;
    
    //创建tcp Socket
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    
    //初始化ListenSocket
    ListenSocket = create_tcp_socket();
    //绑定端口
    socket_bind(ListenSocket, 8001);
    //开始监听
    socket_listen(ListenSocket);

    //TODO: 接收文件大小 分配空间 之后再发送OK命令
    //TODO: 状态码和状态机
    //先发送命令
    char command[MAX_FILE_SIZE] = "OK";
    send_data_to_client(sclient, command);
    //接收客户端的连接
    ClientSocket = socket_accept(ListenSocket);

    do{
        recv_file_info_from_client(ClientSocket, (char*)&file_info);
        write_file_info(filename, &file_info);
    }while(file_info.file_rmd == 0);

    //关闭套接字
    closesocket(ListenSocket);
}

void ftp_quit(SOCKET sclient){
}

void ftp_get(char* filename,SOCKET sclient){}

void ftp_ls(SOCKET sclient){
    send_data_to_client(sclient, get_current_ls());
}

void ftp_cd(char* dirname,SOCKET sclient){
    char* result = change_current_dir(dirname);
    if(result == NULL){
        print_ftp_info(550, "directory not exists!");
        result = "directory not exists!";
    }
    send_data_to_client(sclient, result);
}

void ftp_mkdir(char* dirname,SOCKET sclient){
    char* result;
    if(!create_dir(dirname)){
        print_ftp_info(550, "directory already exists!");
        result = "directory already exists!";
    } else {
        result = "Done!";
    }
    send_data_to_client(sclient, result);
}

void ftp_pwd(SOCKET sclient){
    send_data_to_client(sclient, get_current_dir());
}

void ftp_delete(char* filename,SOCKET sclient){
    char* result;
    if(!delete_file(filename)){
        print_ftp_info(550, "file not exists!");
        result = "file not exists!";
    } else {
        result = "Done!";
    }
    send_data_to_client(sclient, result);
}

int main(){
    struct user_command command;
    //创建tcp Socket
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    
    //初始化ListenSocket
    ListenSocket = create_tcp_socket();
    //绑定端口
    socket_bind(ListenSocket, 8000);
    //开始监听
    socket_listen(ListenSocket);
    while(1){
        //接收客户端的连接
        ClientSocket = socket_accept(ListenSocket);

        while(1){
            //接收客户端的命令
            recv_client_command(ClientSocket, &command);
            //执行命令
            if(strcmp(command.command_name, "get") == 0){
                ftp_get(command.argument, ClientSocket);
            }else if(strcmp(command.command_name, "put") == 0){
                ftp_put(command.argument, ClientSocket);
            }else if(strcmp(command.command_name, "delete") == 0){
                ftp_delete(command.argument, ClientSocket);
            }else if(strcmp(command.command_name, "ls") == 0){
                ftp_ls(ClientSocket);
            }else if(strcmp(command.command_name, "cd") == 0){
                ftp_cd(command.argument, ClientSocket);
            }else if(strcmp(command.command_name, "mkdir") == 0){
                ftp_mkdir(command.argument, ClientSocket);
            }else if(strcmp(command.command_name, "pwd") == 0){
                ftp_pwd(ClientSocket);
            }else if(strcmp(command.command_name, "quit") == 0){
                ftp_quit(ClientSocket);
                break;
            }
        }
    }
}


int recv_client_command(SOCKET sock_control, struct user_command *command)
{	
	int rc = 200;
	char buffer[MAX_FILE_SIZE];
	
    //先将command清空
    memset(command, 0, sizeof(struct user_command));

    // Wait to recieve command
	if ((recv_data_from_client(sock_control, buffer) ) == -1) {
		perror("recv error\n"); 
		return -1;
	}

    if(buffer[0] == '\n'){
        return 0;
    }
    //将用户输入的命令分割成命令码和命令参数
    char *p = strtok(buffer, " ");
    //如果有\n,就除去\n
    if(p[strlen(p)-1] == '\n')
        p[strlen(p)-1] = '\0';
    strcpy(command->command_name, p);
    p = strtok(NULL, " ");
    if(p != NULL){
        if(p[strlen(p)-1] == '\n')
            p[strlen(p)-1] = '\0';
        strcpy(command->argument, p);
    }
    //三个参数,则会报错
    if(strtok(NULL, " ") != NULL){
        print_ftp_info(500, "too many arguments");
        return 0;
    }
    //查看命令是否是合法的
    if(
        strcmp(command->command_name, "get") != 0 
        && strcmp(command->command_name, "put") != 0 
        && strcmp(command->command_name, "delete") != 0 
        && strcmp(command->command_name, "ls") != 0 
        && strcmp(command->command_name, "cd") != 0 
        && strcmp(command->command_name, "mkdir") != 0
        && strcmp(command->command_name, "pwd") != 0
        && strcmp(command->command_name, "quit") != 0){
        print_ftp_info(500, "invalid command");
    }
    printf("command_name:%s, argument:%s\n", command->command_name, command->argument);

    //将命令码转换成小写
    int i = 0;
    while(command->command_name[i] != '\0'){
        command->command_name[i] = tolower(command->command_name[i]);
        i++;
    }
	// return rc;
}