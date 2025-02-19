#pragma once
class SocketIO
{

public:
	void init();




	~SocketIO();


private:
	SOCKET clientSocket{ INVALID_SOCKET };
};

