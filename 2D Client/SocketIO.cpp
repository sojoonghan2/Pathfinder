#include "pch.h"
#include "SocketIO.h"

void SocketIO::init()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

    // 家南 积己
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET); //err_quit("socket()");
}

SocketIO::~SocketIO()
{
    closesocket(clientSocket);
    WSACleanup();
}
