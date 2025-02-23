#include "pch.h"
#include "SocketIO.h"

void SocketIO::init()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

    // ���� ����
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET); //err_quit("socket()");

    SOCKADDR_IN serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT_NUMBER);

    // inet_pton ����Ͽ� IP �ּ� ��ȯ
    if (inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr) != 1) {
        util::DisplayQuitError();
    }

    // connect
    int ret = connect(clientSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == SOCKET_ERROR) {
        util::DisplayQuitError();
    }
    std::println("SocketIO Init completed.");
}

SocketIO::~SocketIO()
{
    closesocket(clientSocket);
    WSACleanup();
}
