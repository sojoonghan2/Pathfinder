#include "pch.h"
#include "SocketIO.h"

constexpr int GRID_CONUT{ 100 };
constexpr int WINDOW_WIDTH{ 800 };
constexpr int WINDOW_HEIGHT{ 800 };

std::queue<std::array<char, BUFFER_SIZE>> packetQueue;

// 플레이어 이동속도, 단위 m/s
constexpr float PLAYER_SPEED{ 5.f };

// 단위 m
constexpr float MAP_SIZE{ 50.f };
constexpr float GRID_SIZE{ MAP_SIZE / static_cast<float>(GRID_CONUT) };


constexpr float GRID_WIDTH_PIXEL
	{ static_cast<float>(WINDOW_WIDTH) / static_cast<float>(GRID_CONUT) };
constexpr float GRID_HEIGHT_PIXEL
	{ static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(GRID_CONUT) };


using StatusType = char;

enum class KeyboardStatus : StatusType
{
	Down = 0x01,
	Up = 0x02,
	Left = 0x04,
	Right = 0x08,
};

class Controller
{
private:
	StatusType keyboardStatus{};

public:
	void applyKeyboardStatus(KeyboardStatus key)
	{
		keyboardStatus |= static_cast<StatusType>(key);
	}

	void disapplyKeyboardStatus(KeyboardStatus key)
	{
		keyboardStatus &= ~static_cast<StatusType>(key);
	}

	bool getKeyBoardStatus(KeyboardStatus key) const
	{
		return keyboardStatus & static_cast<StatusType>(key);
	}
};

class Timer {
private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>; // 밀리초 단위로 변경

	TimePoint lastFrame;
	float deltaTimeMS; // 밀리초 단위의 델타타임

public:
	Timer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // 초 단위 변환

	void updateDeltaTime() {
		TimePoint currentFrame = Clock::now();
		deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
	}
};

class Player
{
private:
	// 단위 m
	int id{-1};
	Vec2f pos{};
	float& x{pos.x};
	float& y{pos.y};
	bool show{ false };
	sf::RectangleShape Square{ sf::Vector2f(GRID_WIDTH_PIXEL, GRID_HEIGHT_PIXEL) };

public:

	Player(const float x, const float y, const sf::Color color, const bool show) :
		pos{ x, y },
		show{show}
	{
		SetFillColor(color);
	}

	Player(const float x, const float y, const sf::Color color) :
		Player{ x, y, color, false }
	{}

	Player() :
		Player{ 25.f, 25.f, sf::Color::Red }
	{}

	void SetFillColor(const sf::Color color)
	{
		Square.setFillColor(color);
	}

	void Update(const Controller& controller, const float delta)
	{
		// 방향을 얻어냄
		float dirX{}, dirY{};
		if (controller.getKeyBoardStatus(KeyboardStatus::Left)) { dirX--; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Right)) { dirX++; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Up)) { dirY--; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Down)) { dirY++; }

		// 방향 정규화
		float length = std::sqrt(dirX * dirX + dirY * dirY);
		if (length != 0) {  // 0으로 나누는 것을 방지
			dirX /= length;
			dirY /= length;
		}

		// 이동거리 계산
		float distance = delta * PLAYER_SPEED;
	
		// 최종 위치 계산
		x += distance * dirX;
		y += distance * dirY;
	}

	void Draw(sf::RenderWindow& window)
	{
		if (not show) return;

		// 윈도우 위치 계산
		float windowX = x / MAP_SIZE * WINDOW_WIDTH;
		float windowY = y / MAP_SIZE * WINDOW_HEIGHT;

		// 윈도우 위치에 표시
		Square.setPosition(windowX, windowY);
		window.draw(Square);
	}


	// getter and setter

	void SetId(const int _id) { id = _id; }

	void SetPosition(Vec2f _pos) { pos = _pos; }

	void SetPosition(const float _x, const float _y)
	{ SetPosition(Vec2f{ _x, _y }); }

	void SetShow(const bool _show) { show = _show; }

};


int main() {
	// 창 생성 (800x800 픽셀, 제목은 "Grid Map")
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Grid Map");
	

	// Init Resources
	sf::RectangleShape line{};
	line.setFillColor(sf::Color::Black);

	Player my_player;
	my_player.SetShow(true);
	Player other_player{ 0.f, 0.f, sf::Color::Blue };
	Player other2_player{ 0.f, 0.f, sf::Color::Yellow };

	// TODO: player를 고쳐야함. 
	// 타이머의 1초마다 클라에서 현 move_packet 전송
	// 타이머의 1초마다 서버에서 전체 player에게 모두 move_packet 전송

	Controller controller;
	SocketIO socket_io;
	socket_io.Init();
	socket_io.Start();


	Timer frame_timer;
	Timer send_timer;
	while (window.isOpen()) {

		// --
		// process packet
		// --
		while (not packetQueue.empty()) {

			auto& buffer = packetQueue.front();
			packet::Header& header = reinterpret_cast<packet::Header&>(buffer);
			switch (header.type) {
			case packet::Type::SC_LOGIN:
			{
				packet::SCLogin packet = reinterpret_cast<packet::SCLogin&>(buffer);
				my_player.SetId(packet.playerId);
				
			}
			break;
			case packet::Type::SC_MOVE_PLAYER:
			{
				packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);
				if (0 == packet.playerId) {
					my_player.SetPosition(packet.x, packet.y);
				}
				else if (1 == packet.playerId) {
					other_player.SetShow(true);
					other_player.SetPosition(packet.x, packet.y);
				}
				else {
					other2_player.SetShow(true);
					other2_player.SetPosition(packet.x, packet.y);
				}
			}
			break;
			default:
			{
				std::println("Packet Error.");
			}
			break;
			}


			packetQueue.pop();
		}


		// ---
		// event
		// ---

		sf::Event event;
		while (window.pollEvent(event)) {

			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;


			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::W:
					controller.applyKeyboardStatus(KeyboardStatus::Up);
					break;
				case sf::Keyboard::S:
					controller.applyKeyboardStatus(KeyboardStatus::Down);
					break;
				case sf::Keyboard::D:
					controller.applyKeyboardStatus(KeyboardStatus::Right);
					break;
				case sf::Keyboard::A:
					controller.applyKeyboardStatus(KeyboardStatus::Left);
					break;
				}
				break;

			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::W:
					controller.disapplyKeyboardStatus(KeyboardStatus::Up);
					break;
				case sf::Keyboard::S:
					controller.disapplyKeyboardStatus(KeyboardStatus::Down);
					break;
				case sf::Keyboard::D:
					controller.disapplyKeyboardStatus(KeyboardStatus::Right);
					break;
				case sf::Keyboard::A:
					controller.disapplyKeyboardStatus(KeyboardStatus::Left);
					break;
				}
				break;
			}
		}


		// --
		// Timer Update
		// --
		frame_timer.updateDeltaTime();
		auto delta = frame_timer.getDeltaTimeSeconds();


		// ---
		// update
		// ---

		my_player.Update(controller, delta);


		// ---
		// draw
		// --- 
		window.clear(sf::Color::White); // 흰색 배경

		// 수평선 그리기
		for (int i = 0; i <= GRID_CONUT; ++i) {
			line.setSize(sf::Vector2f(WINDOW_HEIGHT, 1));
			line.setPosition(0, i * GRID_WIDTH_PIXEL);
			window.draw(line);
		}

		// 수직선 그리기 (line을 90도 회전)
		line.setSize(sf::Vector2f(1, WINDOW_HEIGHT));
		for (int i = 0; i <= GRID_CONUT; ++i) {
			line.setPosition(i * GRID_HEIGHT_PIXEL, 0);
			window.draw(line);
		}

		my_player.Draw(window);
		other_player.Draw(window);

		window.display();
	}

	return 0;
}