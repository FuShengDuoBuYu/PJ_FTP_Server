#include "ftp_server.h"
#include "common.h"

void ftp_put(MsgHeader* msgHeader,SOCKET sclient){
    if(msgHeader->msgID = MSG_FILEINFO){
        char* dir = get_current_dir();
        memset(file_name, 0, MAX_FILE_SIZE);
        strcat(file_name, dir);
        strcat(file_name, "\\");
        strcat(file_name, msgHeader->info.commandInfo.argument);
        FILE *fp = fopen(file_name, "r");
        if(fp != NULL){
            msgHeader->msgID = MSG_INVALID_FILENAME;
            msgHeader->msgType = MSGTYPE_PUT;
            strcpy(msgHeader->info.commandInfo.argument, "file already exists!");
            send_data_to_client(sclient, (char *)msgHeader);
            return;
        }
    }
    // printf("file_name");
        
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
    msgHeader->msgID = MSG_READY;
    msgHeader->msgType = MSGTYPE_PUT;
    send_data_to_client(sclient, (char*)msgHeader);
    //TODO: 接收文件大小 分配空间 之后再发送OK命令
    //TODO: 状态码和状态机
    //先发送命令
    // char command[MAX_FILE_SIZE] = "OK";
    // send_data_to_client(sclient, command);
    //接收客户端的连接
    ClientSocket = socket_accept(ListenSocket);
    do{
        recv_file_info_from_client(ClientSocket, msgHeader);
        FileType fileType = (msgHeader->msgID == MSG_SEND_BINARY) ? BINARY_FILE : TEXT_FILE;
// printf("fileType: %d\n", fileType);
        write_file_info(file_name, &msgHeader->info.fileData, fileType);
        msgHeader->msgID = msgHeader->msgID;
        if (msgHeader->info.fileData.file_tag == 1){
            msgHeader->msgID = MSG_SUCCESSED;
        }
        send_file_info_to_client(ClientSocket, (char *)msgHeader);
    }while(msgHeader->info.fileData.file_tag == 0);
    //关闭套接字
    
    closesocket(ListenSocket);
}

void ftp_quit(SOCKET sclient){
}

void ftp_get(char* filename,SOCKET sclient){
    //接收Client传过来的文件名
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgType = MSGTYPE_GET;
    msgHeader.msgID = MSG_FILEINFO;
    strcpy(msgHeader.info.commandInfo.argument, filename);
    printf("get filename");

    // // 把filename变量按空格分开，分别存入file_name和IP
    // char file_name[MAX_FILE_SIZE];
    // char clientIP[MAX_FILE_SIZE];
    // memset(file_name, 0, MAX_FILE_SIZE);
    // memset(clientIP, 0, MAX_FILE_SIZE);
    // char *p = strtok(filename, " ");
    // //如果有\n,就除去\n
    // if(p[strlen(p)-1] == '\n')
    //     p[strlen(p)-1] = '\0';
    // strcpy(file_name, p);
    // p = strtok(NULL, " ");
    // if(p != NULL){
    //     if(p[strlen(p)-1] == '\n')
    //         p[strlen(p)-1] = '\0';
    //     strcpy(clientIP, p);
    // }
// printf("file_name: %s, clientIP: %s\n", file_name, clientIP);
    //1.判断有无文件，返回相应指令
    if (file_exists(filename) == 0){
        msgHeader.msgID = MSG_INVALID_FILENAME;
        strcpy(msgHeader.info.commandInfo.argument, "file not exists!");
        send_data_to_client(sclient, (char *)&msgHeader);
        return;
    } 

    //存在文件，返回确认数据并打开端口等待连接。
    msgHeader.msgType = MSGTYPE_GET;
    msgHeader.msgID = MSG_READY;
    
    //创建tcp Socket
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    
    //初始化ListenSocket
    ListenSocket = create_tcp_socket();
    //绑定端口,发送返回数据
    socket_bind(ListenSocket, 8002);
    
    //开始监听
    socket_listen(ListenSocket);
    send_data_to_client(sclient, (char*)&msgHeader);
    ClientSocket = socket_accept(ListenSocket);  
    
    //连接成功后开始发送文件
    send_file_to_client(ClientSocket, filename);
    closesocket(ListenSocket);
    printf("file uploaded!\n");
}

void ftp_ls(SOCKET sclient){
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgID = MSG_SUCCESSED;
    msgHeader.msgType = MSGTYPE_LS;
    strcpy(msgHeader.info.commandInfo.argument, get_current_ls());
    send_data_to_client(sclient, (char*)&msgHeader);
}

void ftp_cd(char* dirname,SOCKET sclient){
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgType = MSGTYPE_CD;
    char* res = change_current_dir(dirname);
    if(res == NULL){
        msgHeader.msgID = MSG_FAILED;
        strcpy(msgHeader.info.commandInfo.argument, "directory not exists!");
    } else {
        msgHeader.msgID = MSG_SUCCESSED;
        strcpy(msgHeader.info.commandInfo.argument, res);
    }
    send_data_to_client(sclient, (char*)&msgHeader);
}

void ftp_mkdir(char* dirname,SOCKET sclient){
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgType = MSGTYPE_MKDIR;
    if(!create_dir(dirname)){
        print_ftp_info(550, "directory already exists!");
        msgHeader.msgID = MSG_FAILED;
        strcpy(msgHeader.info.commandInfo.argument, "directory already exists!");
    } else {
        msgHeader.msgID = MSG_SUCCESSED;
        strcpy(msgHeader.info.commandInfo.argument, "directory created!");
    }
    send_data_to_client(sclient, (char*)&msgHeader);
}

void ftp_pwd(SOCKET sclient){
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgID = MSG_SUCCESSED;
    msgHeader.msgType = MSGTYPE_PWD;
    strcpy(msgHeader.info.commandInfo.argument, get_current_dir());
    send_data_to_client(sclient, (char*)&msgHeader);
}

void ftp_delete(char* filename,SOCKET sclient){
    MsgHeader msgHeader;
    memset(&msgHeader, 0, sizeof(msgHeader));
    msgHeader.msgType = MSGTYPE_DELETE;
    if(!delete_file(filename)){
        print_ftp_info(550, "file not exists!");
        msgHeader.msgID = MSG_FAILED;
        strcpy(msgHeader.info.commandInfo.argument, "file not exists!");
    } else {
        msgHeader.msgID = MSG_SUCCESSED;
        strcpy(msgHeader.info.commandInfo.argument, "file deleted!");
    }
    send_data_to_client(sclient, (char*)&msgHeader);
}

int main(){
    MsgHeader msgHeader;
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
        int quit = 0;
        while(!quit){
            //接收客户端的命令
            recv_client_command(ClientSocket, &msgHeader);
            printf("msgID: %d\n", msgHeader.msgID);
            switch (msgHeader.msgType)
            {
                case MSGTYPE_PUT:
                    /* code */
                    ftp_put(&msgHeader, ClientSocket);
                    break;
                case MSGTYPE_GET:
                    /* code */
                    ftp_get(msgHeader.info.commandInfo.argument, ClientSocket);
                    break;
                case MSGTYPE_LS:
                    /* code */
                    ftp_ls(ClientSocket);
                    break;
                case MSGTYPE_CD:
                    /* code */
                    ftp_cd(msgHeader.info.commandInfo.argument, ClientSocket);
                    break;
                case MSGTYPE_MKDIR:
                    /* code */
                    ftp_mkdir(msgHeader.info.commandInfo.argument, ClientSocket);
                    break;
                case MSGTYPE_PWD:
                    /* code */
                    ftp_pwd(ClientSocket);
                    break;
                case MSGTYPE_DELETE:
                    /* code */
                    ftp_delete(msgHeader.info.commandInfo.argument, ClientSocket);
                    break;
                case MSGTYPE_QUIT:
                    /* code */
                    ftp_quit(ClientSocket);
                    quit = 1;
                    break;
                default:
                    break;
            }
        }
    }
}


int recv_client_command(SOCKET sock_control, MsgHeader * msgHeader)
{	
	int rc = 200;
	
    //先将command清空
    memset(msgHeader, 0, sizeof(MsgHeader));

    // Wait to recieve command
	if ((recv_data_from_client(sock_control, (char *)msgHeader) ) == -1) {
		perror("recv error\n"); 
		return -1;
	}
}