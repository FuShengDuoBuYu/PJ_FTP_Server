//定义一些常用的,在整个程序中会用到的一些内容

#ifndef COMMON_H
#define COMMON_H
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>

struct user_command{
    //命令后的参数
    char argument[255];
    //命令名称
    char command_name[255];
};

//创建一个TCP连接
SOCKET create_tcp_socket();

//绑定本机端口
int bind_socket_local_port(SOCKET sclient, int port);

//连接到服务端
int connect_to_server(SOCKET sclient, char *ip, int port);

//发送数据
int send_data_to_server(SOCKET sclient, char *sendbuf);

//接收数据
int recv_data_from_server(SOCKET sclient, char *recvbuf);

//关闭连接
int close_socket(SOCKET sclient);
# endif