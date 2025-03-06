#include "pch.h"
#include "NetworkManager.h"
#include "SocketIO.h"

NETWORK_START

void NetworkManager::Init()
{
	socketIO.Init();
	socketIO.Start();
}

void NetworkManager::Update()
{
	
}




NETWORK_END