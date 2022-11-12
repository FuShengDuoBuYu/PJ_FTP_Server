# include "common.h"
# include "file_util.h"

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

int socket_bind(SOCKET sclient, int port){
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

int socket_listen(SOCKET sclient){
    int iResult;
    iResult = listen(sclient, 5);
    if(iResult == SOCKET_ERROR){
        printf("listen error !");
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    return 1;
}

SOCKET socket_accept(SOCKET listenSocket){
    int iResult;
    SOCKET clientSocket = INVALID_SOCKET;

    // Accept a client socket
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    return clientSocket;
}

int send_data_to_client(SOCKET sclient, char *sendbuf){
    int iResult;
    iResult = send(sclient, sendbuf, sizeof(MsgHeader), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    printf("Bytes Sent: %ld\n", iResult);
    return 1;
}

int recv_data_from_client(SOCKET sclient, char *recvbuf){
    int iResult;
    iResult = recv(sclient, recvbuf, sizeof(MsgHeader), 0);
    if(iResult == SOCKET_ERROR){
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    printf("Bytes Recv: %ld\n", iResult);
    return 1;
}

int send_file_info_to_client(SOCKET sclient, MsgHeader *sendbuf){
    int iResult;
    iResult = send(sclient, (char *)sendbuf, sizeof(MsgHeader), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    printf("Bytes Sent: %ld\n", iResult);
    return 1;
}

int recv_file_info_from_client(SOCKET sclient, MsgHeader *recvbuf){
    int iResult;
    iResult = recv(sclient, (char*)recvbuf, sizeof(MsgHeader), 0);
    if(iResult == SOCKET_ERROR){
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(sclient);
        WSACleanup();
        return 0;
    }
    printf("Bytes Recv: %ld\n", iResult);
    return 1;
}

int close_socket(SOCKET sclient){
    closesocket(sclient);
    WSACleanup();
    return 1;
}