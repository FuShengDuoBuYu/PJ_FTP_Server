# include "common.h"

SOCKET create_tcp_socket(){
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0){
        printf("WSAStartup error!");
        return 0;
    }
    SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return sclient;
}

int bind_socket_local_port(SOCKET sclient, int port){
    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.S_un.S_addr = 0;
    int iResult;
    iResult = bind(sclient, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if(iResult == SOCKET_ERROR){
        printf("bind error !");
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    return 1;
}

int connect_to_server(SOCKET sclient, char *ip, int port){
    int iResult;
    struct sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.S_un.S_addr = inet_addr(ip);
    iResult = connect(sclient, (SOCKADDR *)&serAddr, sizeof(SOCKADDR));
    if(iResult == SOCKET_ERROR){
        printf("connect error !\n");
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    return 1;
}

int send_data_to_server(SOCKET sclient, char *sendbuf){
    int iResult;
    iResult = send(sclient, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    printf("Bytes Sent: %ld\n", iResult);
    return 1;
}

int recv_data_from_server(SOCKET sclient, char *recvbuf){
    int iResult;
    iResult = recv(sclient, recvbuf, 1024, 0);
    if(iResult == SOCKET_ERROR){
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
}

int close_socket(SOCKET sclient){
    closesocket(sclient);
    WSACleanup();
    return 1;
}