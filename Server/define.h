#pragma once


struct Vec2f
{
	float x{ 0.f };
	float y{ 0.f };

	Vec2f() = default;
	Vec2f(const float x, const float y) :
		x{ x },
		y{ y }
	{}

	Vec2f operator+(const Vec2f& rhs) const
	{
		return Vec2f{ x + rhs.x, y + rhs.y };
	}

	Vec2f operator-(const Vec2f& rhs) const
	{
		return Vec2f{ x - rhs.x, y - rhs.y };
	}

	void Normalize()
	{
		float length = std::sqrt(x * x + y * y);
		if (length > 0.f) {
			x = x / length;
			y = y / length;
		}
		else {
			x = 0.f;
			y = 0.f;
		}
	}

	float Dot(const Vec2f& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}
};

struct ClientIdInfo
{
	int clientId{ -1 };
	int playerId{ -1 };
	int roomId{ -1 };
};

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

enum class IOState
{
	NONE,
	CONNECT,
	INGAME,
	DISCONNECT
};


struct alignas(64) OverlappedEx
{
	WSAOVERLAPPED	over;					// overlapped IO 구조체
	SOCKET			clientSocket{ INVALID_SOCKET }; // 클라이언트 소켓
	WSABUF			wsabuf{};					// 작업 버퍼
	char			dataBuffer[BUFFER_SIZE]{};		// 데이터 버퍼
	IOOperation		operation{};				// 동작 종류

	// recv 전용 생성자. 
	OverlappedEx() :
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUFFER_SIZE;
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
	}

	// send 전용 생성자
	OverlappedEx(unsigned char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
		operation = IOOperation::SEND;
		memcpy(dataBuffer, packet, packet[0]);
	}
};

struct Session
{
	OverlappedEx	overEx;								// RECV에 사용할 Overlapped 변수
	SOCKET			clientSocket{ INVALID_SOCKET };	
	int				currentDataSize{};					// 패킷 재조립을 위한 남은 데이터 수 
	ClientIdInfo	clientIdInfo{};

	// atomic하게 해야할듯?
	IOState			ioState{ IOState::NONE };

	// unordered_map을 사용하기 위해서 기본 생성자가 있어야 함.
	// 사실상 미사용.
	Session()
	{
		exit(-1);
	}

	Session(SOCKET socket, int client_id) :
		clientSocket{ socket },
		currentDataSize{ 0 },
		ioState{ IOState::CONNECT }
	{
		clientIdInfo.clientId = client_id;
		overEx.clientSocket = socket;
	}

	~Session()
	{
		// 연결 종료 시 게임이랑 다른 플레이어들에게 알려주어야 할듯.
		closesocket(clientSocket);
	}
};


// 싱글턴
#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()