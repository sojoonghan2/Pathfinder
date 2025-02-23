#include "pch.h"
#include "SocketIO.h"

constexpr int GRID_CONUT{ 100 };
constexpr int WINDOW_WIDTH{ 800 };
constexpr int WINDOW_HEIGHT{ 800 };



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

class GameLoop {
private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>; // �и��� ������ ����

	TimePoint lastFrame;
	float deltaTimeMS; // �и��� ������ ��ŸŸ��

public:
	GameLoop() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // �� ���� ��ȯ

	void updateDeltaTime() {
		TimePoint currentFrame = Clock::now();
		deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
	}
};

class Player
{
private:
	// ���� m
	float x{};
	float y{};
	sf::RectangleShape Square{ sf::Vector2f(GRID_WIDTH_PIXEL, GRID_HEIGHT_PIXEL) };

public:

	Player(float x, float y, sf::Color color) :
		x{x},
		y{y}
	{
		setFillColor(color);
	}

	Player(float x, float y) :
		Player{ x, y, sf::Color::Red }
	{}

	Player() :
		Player{ 25.f, 25.f }
	{}



	void setFillColor(sf::Color color)
	{
		Square.setFillColor(color);
	}

	void update(Controller& controller, float delta)
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

	void draw(sf::RenderWindow& window)
	{
		// ������ ��ġ ���
		float windowX = x / MAP_SIZE * WINDOW_WIDTH;
		float windowY = y / MAP_SIZE * WINDOW_HEIGHT;

		// ������ ��ġ�� ǥ��
		Square.setPosition(windowX, windowY);
		window.draw(Square);
	}
};


int main() {
	// â ���� (800x800 �ȼ�, ������ "Grid Map")
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Grid Map");
	

	// Init Resources
	sf::RectangleShape line{};
	line.setFillColor(sf::Color(sf::Color::Black));

	Player my_player;
	Player other_player{ 20.f, 20.f, sf::Color::Blue };

	GameLoop timer;
	Controller controller;
	SocketIO socket_io;
	socket_io.init();


	while (window.isOpen()) {

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
		timer.updateDeltaTime();
		auto delta = timer.getDeltaTimeSeconds();


		// ---
		// update
		// ---

		my_player.update(controller, delta);


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

		my_player.draw(window);
		other_player.draw(window);

		window.display();
	}

	return 0;
}