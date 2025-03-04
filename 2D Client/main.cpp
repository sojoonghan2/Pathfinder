#include "pch.h"
#include "SocketIO.h"

constexpr int GRID_CONUT{ 100 };
constexpr int WINDOW_WIDTH{ 800 };
constexpr int WINDOW_HEIGHT{ 800 };

std::queue<std::array<char, BUFFER_SIZE>> packetQueue;

// �÷��̾� �̵��ӵ�, ���� m/s
constexpr float PLAYER_SPEED{ 5.f };

// ���� m
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
	using MilliSeconds = std::chrono::duration<float, std::milli>; // �и��� ������ ����

	TimePoint lastFrame;
	float deltaTimeMS; // �и��� ������ ��ŸŸ��

public:
	Timer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // �� ���� ��ȯ

	void updateDeltaTime() {
		TimePoint currentFrame = Clock::now();
		deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
	}

	float PeekDeltaTime() const {
		TimePoint currentFrame = Clock::now();
		return std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
	}
};

class Player
{
private:
	// ���� m
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
		// ������ ��
		float dirX{}, dirY{};
		if (controller.getKeyBoardStatus(KeyboardStatus::Left)) { dirX--; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Right)) { dirX++; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Up)) { dirY--; }
		if (controller.getKeyBoardStatus(KeyboardStatus::Down)) { dirY++; }

		// ���� ����ȭ
		float length = std::sqrt(dirX * dirX + dirY * dirY);
		if (length != 0) {  // 0���� ������ ���� ����
			dirX /= length;
			dirY /= length;
		}

		// �̵��Ÿ� ���
		float distance = delta * PLAYER_SPEED;
	
		// ���� ��ġ ���
		x += distance * dirX;
		y += distance * dirY;
	}

	void Draw(sf::RenderWindow& window)
	{
		if (not show) return;

		// ������ ��ġ ���
		float windowX = x / MAP_SIZE * WINDOW_WIDTH;
		float windowY = y / MAP_SIZE * WINDOW_HEIGHT;

		// ������ ��ġ�� ǥ��
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
	// â ���� (800x800 �ȼ�, ������ "Grid Map")
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Grid Map");
	

	// Init Resources
	sf::RectangleShape line{};
	line.setFillColor(sf::Color::Black);

	// Ÿ�̸��� 1�ʸ��� Ŭ�󿡼� �� move_packet ����
	// Ÿ�̸��� 1�ʸ��� �������� ��ü player���� ��� move_packet ����

	std::array<Player, 3> players{};
	for (auto& player : players) {
		player.SetFillColor(sf::Color::Black);
	}


	int my_id{ -1 };

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
				players[packet.playerId].SetFillColor(sf::Color::Red);
				my_id = packet.playerId;
			}
			break;
			case packet::Type::SC_MOVE_PLAYER:
			{
				packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);
				players[packet.playerId].SetShow(true);
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
		// Update
		// --

		// player
		frame_timer.updateDeltaTime();
		auto delta = frame_timer.getDeltaTimeSeconds();

		if (0 <= my_id && my_id <= 2) {
			players[my_id].Update(controller, delta);

			// send
			if (send_timer.PeekDeltaTime() > 100.f) {
				send_timer.updateDeltaTime();
				auto pos = players[my_id].GetPosition();
				socket_io.DoSend<packet::CSMovePlayer>(my_id, pos.x, pos.y);
			}
		}


		// ---
		// draw
		// --- 
		window.clear(sf::Color::White); // ��� ���

		// ���� �׸���
		for (int i = 0; i <= GRID_CONUT; ++i) {
			line.setSize(sf::Vector2f(WINDOW_HEIGHT, 1));
			line.setPosition(0, i * GRID_WIDTH_PIXEL);
			window.draw(line);
		}

		// ������ �׸��� (line�� 90�� ȸ��)
		line.setSize(sf::Vector2f(1, WINDOW_HEIGHT));
		for (int i = 0; i <= GRID_CONUT; ++i) {
			line.setPosition(i * GRID_HEIGHT_PIXEL, 0);
			window.draw(line);
		}

		for (auto& player : players) {
			player.Draw(window);
		}

		window.display();
	}

	return 0;
}