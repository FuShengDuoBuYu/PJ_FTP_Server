//定义一些常用的,在整个程序中会用到的一些内容

#ifndef COMMON_H
#define COMMON_H
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include "file_util.h"

// 定义标记
enum MSGTAG
{
	MSG_FILEINFO       = 1,        // 文件名称                服务器使用
	MSG_DIRINFO       = 2,        // 文件夹名称
	MSG_READY     = 3,        // 准备接受                客户端使用
	MSG_SEND       = 4,        // 发送                    服务器使用
	MSG_SUCCESSED      = 5,        // 传输完成                两者都使用
	MSG_FAILED = 6,         // 告诉客户端文件找不到    客户端使用
    MSG_INVALID_FILENAME = 7, // 告诉客户端文件名不合法  
    MSG_INVALID_DIR = 8 // 告诉客户端文件名不合法  
};

// 定义标命令
enum MSGTYPE
{
	MSGTYPE_GET = 1,        // 获取文件
    MSGTYPE_PUT = 2,        // 上传文件
    MSGTYPE_LS  = 3,        // 列出文件
    MSGTYPE_CD  = 4,        // 切换目录
    MSGTYPE_PWD = 5,        // 显示当前目录
    MSGTYPE_MKDIR= 6,         // 退出
    MSGTYPE_DELETE= 7,         // 退出
    MSGTYPE_QUIT= 8         // 退出
};

typedef struct                  // 封装消息头
{
	enum MSGTAG msgID;             // 当前消息标记   4
    enum MSGTYPE msgType;          // 当前消息类型   4
	union MyUnion
	{
		struct Mystruct
		{
			int fileSize;           // 文件大小  4
			char argument[MAX_FILE_SIZE];     // 文件名    256
		}commandInfo;
		FileInfo fileData;          // 文件数据
	} info;
 
}MsgHeader;

//创建一个TCP连接
SOCKET create_tcp_socket();

//绑定本机端口
int socket_bind(SOCKET sclient, int port);

//开始监听
int socket_listen(SOCKET sclient);

//接受客户端的连接
SOCKET socket_accept(SOCKET listenSocket);

//发送数据
int send_data_to_client(SOCKET sclient, char *sendbuf);

//接收数据
int recv_data_from_client(SOCKET sclient, char *recvbuf);

//生成FileInfo并发送
int send_file_to_client(SOCKET sclient, char *filename);

//发送FileInfo
int send_file_info_to_client(SOCKET sclient, MsgHeader *sendbuf);

//接收FileInfo
int recv_file_info_from_client(SOCKET sclient, MsgHeader *recvbuf);

//关闭连接
int close_socket(SOCKET sclient);
# endif