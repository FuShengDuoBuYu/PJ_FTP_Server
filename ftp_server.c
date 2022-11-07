#include "ftp_server.h"

void read_command_from_cmdline(char *cmdline, struct user_command *command){
    //读取用户输入的命令
    while(1){
        printf("ftp> ");
        //先将command清空
        memset(command, 0, sizeof(struct user_command));
        fgets(cmdline,MAX_CMDLINE,stdin);
        if(cmdline[0] == '\n')
            continue;
        //将用户输入的命令分割成命令码和命令参数
        char *p = strtok(cmdline, " ");
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
            continue;
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
            continue;
        }
        //将命令码转换成小写
        int i = 0;
        while(command->command_name[i] != '\0'){
            command->command_name[i] = tolower(command->command_name[i]);
            i++;
        }
        break;
    }
}

void ftp_put(char* filename,SOCKET sclient){
    if(file_exists(filename) == 0){
        print_ftp_info(550, "file not exists");
        return;
    }
    char send_buffer[MAX_FILE_SIZE];
    int send_buffer_index = 0;
    memset(send_buffer, 0, sizeof(send_buffer));
    //获取文件的内容,每次取1024字节,然后发送,直到最后一个发送的不到1024,代表文件已经发送完毕
    int last_send_size = MAX_FILE_SIZE;
    int enter_count = 0;
    while(last_send_size == MAX_FILE_SIZE){
        last_send_size = get_file_content(filename, send_buffer, send_buffer_index,&enter_count);
        send_data_to_server(sclient, send_buffer);
        memset(send_buffer, 0, sizeof(send_buffer));
        send_buffer_index++;
    }
    //接收数据
    char recvbuf[MAX_FILE_SIZE];
    int recv_result = recv_data_from_server(sclient, recvbuf);
    printf("%s\n", recvbuf);
}

void ftp_quit(SOCKET sclient){
    closesocket(sclient);
    printf("bye~");
}

void ftp_get(char* filename,SOCKET sclient){}

void ftp_ls(SOCKET sclient){
    //先发送命令
    char* pwd_command = "ls";
    send_data_to_server(sclient, pwd_command);
    char recvbuf[MAX_FILE_SIZE];
    //先把recvbuf清空
    memset(recvbuf, 0, sizeof(recvbuf));
    int recv_result = recv_data_from_server(sclient, recvbuf);
    printf("%s\n", recvbuf);
}

void ftp_cd(char* dirname,SOCKET sclient){}

void ftp_mkdir(char* dirname,SOCKET sclient){}

void ftp_pwd(SOCKET sclient){
    //先发送命令
    char* pwd_command = "pwd";
    send_data_to_server(sclient, pwd_command);
    char recvbuf[MAX_FILE_SIZE];
    //先把recvbuf清空
    memset(recvbuf, 0, sizeof(recvbuf));
    int recv_result = recv_data_from_server(sclient, recvbuf);
    printf("%s\n", recvbuf);
}

void ftp_delete(char* filename,SOCKET sclient){}

int main(){
    char cmdline[MAX_CMDLINE];
    struct user_command command;
    //创建tcp Socket
    SOCKET sclient = INVALID_SOCKET;
    sclient = create_tcp_socket();
    if(sclient == INVALID_SOCKET)
        printf("socket error !");
    //绑定本机的端口
    bind_socket_local_port(sclient, 8000);
    //连接server端
    int connect_result = connect_to_server(sclient, "127.0.0.1",8000);
    int count = 0;
    //连接失败进行轮询,轮询最多10次
    // while(connect_result == 0&&count < 10){
    //     Sleep(1000);
    //     printf("retrying...\n");
    //     connect_result = connect_to_server(sclient, "192.168.56.1",8080);
    //     count++;
    // }
    //读取命令并执行
    while(1 && connect_result){
        read_command_from_cmdline(cmdline,&command);
        if(strcmp(command.command_name, "get") == 0){
            ftp_get(command.argument,sclient);
        }
        if(strcmp(command.command_name, "put") == 0){
            ftp_put(command.argument, sclient);
        }
        if(strcmp(command.command_name, "delete") == 0){
            ftp_delete(command.argument, sclient);
        }
        if(strcmp(command.command_name, "ls") == 0){
            ftp_ls(sclient);
        }
        if(strcmp(command.command_name, "cd") == 0){
            ftp_cd(command.argument, sclient);
        }
        if(strcmp(command.command_name, "mkdir") == 0){
            ftp_mkdir(command.argument, sclient);
        }
        if(strcmp(command.command_name, "pwd") == 0){
            ftp_pwd(sclient);
        }
        if(strcmp(command.command_name, "quit") == 0){
            ftp_quit(sclient);
            break;
        }
    }
    return 0;
}