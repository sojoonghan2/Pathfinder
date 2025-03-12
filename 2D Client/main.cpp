#include "pch.h"
#include "SocketIO.h"
#include "NetworkTimer.h"

constexpr int GRID_CONUT{ 100 };
constexpr int WINDOW_WIDTH{ 800 };
constexpr int WINDOW_HEIGHT{ 800 };

std::queue<std::array<char, BUFFER_SIZE>> packetQueue;

constexpr float GRID_SIZE{ MAP_SIZE_M / static_cast<float>(GRID_CONUT) };


constexpr float GRID_WIDTH_PIXEL
	{ static_cast<float>(WINDOW_WIDTH) / static_cast<float>(GRID_CONUT) };
constexpr float GRID_HEIGHT_PIXEL
	{ static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(GRID_CONUT) };


using StatusType = unsigned char;

enum class KeyStatus : StatusType
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
	void applyKeyboardStatus(KeyStatus key)
	{
		keyboardStatus |= static_cast<StatusType>(key);
	}

	void disapplyKeyboardStatus(KeyStatus key)
	{
		keyboardStatus &= ~static_cast<StatusType>(key);
	}

	bool getKeyBoardStatus(KeyStatus key) const
	{
		return keyboardStatus & static_cast<StatusType>(key);
	}
};

class Player
{
private:
	// 단위 m
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
		sf::FloatRect bounds = Square.getLocalBounds();
		Square.setOrigin(bounds.width / 2, bounds.height / 2);
	}

	Player(const float x, const float y, const sf::Color color) :
		Player{ x, y, color, false }
	{}

	Player() :
		Player{ 0.f, 0.f, sf::Color::Red }
	{}

	void SetFillColor(const sf::Color color)
	{
		Square.setFillColor(color);
	}

	void Update(const Controller& controller, const float delta)
	{
		// 방향을 얻어냄
		float dirX{}, dirY{};
		if (controller.getKeyBoardStatus(KeyStatus::Left)) { dirX--; }
		if (controller.getKeyBoardStatus(KeyStatus::Right)) { dirX++; }
		if (controller.getKeyBoardStatus(KeyStatus::Up)) { dirY--; }
		if (controller.getKeyBoardStatus(KeyStatus::Down)) { dirY++; }

		// 방향 정규화
		float length = std::sqrt(dirX * dirX + dirY * dirY);
		if (length != 0) {  // 0으로 나누는 것을 방지
			dirX /= length;
			dirY /= length;
		}

		// 이동거리 계산
		float distance = delta * PLAYER_SPEED_MPS;
	
		// 최종 위치 계산, dirextx 좌표계로 변환
		x += distance * dirX;
		x = std::max(x, -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		x = std::min(x, (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		y += distance * dirY * -1.f;
		y = std::max(y, -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		y = std::min(y, (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);



	}

	void Draw(sf::RenderWindow& window)
	{
		if (not show) return;

		// 윈도우 위치 계산
		float windowX = x / MAP_SIZE_M * WINDOW_WIDTH + (WINDOW_WIDTH * 0.5f);
		float windowY = -y / MAP_SIZE_M * WINDOW_HEIGHT + (WINDOW_HEIGHT * 0.5f);

		// 윈도우 위치에 표시
		Square.setPosition(windowX, windowY);
		window.draw(Square);
	}


	// getter and setter


	void SetPosition(Vec2f _pos) { pos = _pos; }

	void SetPosition(const float _x, const float _y)
	{ SetPosition(Vec2f{ _x, _y }); }

	void SetShow(const bool _show) { show = _show; }

	Vec2f GetPosition() const { return pos; }

};


int main() {
	// 창 생성 (800x800 픽셀, 제목은 "Grid Map")
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Grid Map");
	

	// Init Resources
	sf::RectangleShape line{};
	line.setFillColor(sf::Color::Black);

	// 타이머의 1초마다 클라에서 현 move_packet 전송
	// 타이머의 1초마다 서버에서 전체 player에게 모두 move_packet 전송


	std::unordered_map<int, Player> players{};
	int my_id = -1;

	Controller controller;
	SocketIO socket_io;
	socket_io.Init();
	socket_io.Start();


	NetworkTimer frame_timer;
	NetworkTimer send_timer;

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
				players[packet.playerId].SetFillColor(sf::Color::Red);
				players[packet.playerId].SetShow(true);
				my_id = packet.playerId;
			}
			break;
			case packet::Type::SC_MOVE_PLAYER:
			{
				packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);
				if (not players.contains(packet.playerId)) {
					players[packet.playerId].SetFillColor(sf::Color::Black);
					players[packet.playerId].SetShow(true);
				}
				players[packet.playerId].SetPosition(packet.x, packet.y);
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
				case sf::Keyboard::W: case sf::Keyboard::Up:
					controller.applyKeyboardStatus(KeyStatus::Up);
					break;
				case sf::Keyboard::S: case sf::Keyboard::Down:
					controller.applyKeyboardStatus(KeyStatus::Down);
					break;
				case sf::Keyboard::D: case sf::Keyboard::Right:
					controller.applyKeyboardStatus(KeyStatus::Right);
					break;
				case sf::Keyboard::A: case sf::Keyboard::Left:
					controller.applyKeyboardStatus(KeyStatus::Left);
					break;
				}
				break;

			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::W: case sf::Keyboard::Up:
					controller.disapplyKeyboardStatus(KeyStatus::Up);
					break;
				case sf::Keyboard::S: case sf::Keyboard::Down:
					controller.disapplyKeyboardStatus(KeyStatus::Down);
					break;
				case sf::Keyboard::D: case sf::Keyboard::Right:
					controller.disapplyKeyboardStatus(KeyStatus::Right);
					break;
				case sf::Keyboard::A: case sf::Keyboard::Left:
					controller.disapplyKeyboardStatus(KeyStatus::Left);
					break;
				}
				break;
			}
		}




		// --
		// Update
		// --

		// player
		frame_timer.updateDeltaTime();
		auto delta = frame_timer.getDeltaTimeSeconds();

		if (-1 != my_id) {
			players[my_id].Update(controller, delta);

			// send
			if (send_timer.PeekDeltaTime() > 50.f) {
				send_timer.updateDeltaTime();
				auto pos = players[my_id].GetPosition();
				socket_io.DoSend<packet::CSMovePlayer>(my_id, pos.x, pos.y);
			}
		}


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

		for (auto& [_, player] : players) {
			player.Draw(window);
		}

		window.display();
	}

	return 0;
}