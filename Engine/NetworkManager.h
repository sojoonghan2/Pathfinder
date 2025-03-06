#pragma once

NETWORK_START

class NetworkManager
{
	DECLARE_SINGLE(NetworkManager);


public:
	void Init();
	void Update();

private:
	network::SocketIO socketIO;
	

};

NETWORK_END

