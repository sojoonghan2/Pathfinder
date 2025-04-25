#include "pch.h"
#include "SocketIO.h"
#include "Timer.h"



constexpr int GRID_CONUT{ 100 };
constexpr int WINDOW_WIDTH{ 800 };
constexpr int WINDOW_HEIGHT{ 800 };

std::queue<std::array<char, BUFFER_SIZE>> packetQueue;

constexpr float GRID_SIZE{ MAP_SIZE_M / static_cast<float>(GRID_CONUT) };


constexpr float GRID_WIDTH_PIXEL
	{ static_cast<float>(WINDOW_WIDTH) / static_cast<float>(GRID_CONUT) };
constexpr float GRID_HEIGHT_PIXEL
	{ static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(GRID_CONUT) };

Vec2f WindowToGame(const Vec2f& pos)
{
	float x = (pos.x - (WINDOW_WIDTH * 0.5f)) * MAP_SIZE_M / WINDOW_WIDTH;
	float y = -(pos.y - (WINDOW_HEIGHT * 0.5f)) * MAP_SIZE_M / WINDOW_HEIGHT;
	return Vec2f{ x, y };
}

Vec2f GameToWindow(const Vec2f& pos)
{
	float windowX = pos.x / MAP_SIZE_M * WINDOW_WIDTH + (WINDOW_WIDTH * 0.5f);
	float windowY = -pos.y / MAP_SIZE_M * WINDOW_HEIGHT + (WINDOW_HEIGHT * 0.5f);
	return Vec2f{ windowX, windowY };
}

void NormalizeVec(Vec2f& pos)
{
	float length = std::sqrt(pos.x * pos.x + pos.y * pos.y);
	if (length > 0.f) {
		pos.x /= length;
		pos.y /= length;
	}
}


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
	Vec2f mousePos{};

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

	Vec2f getMousePos() const
	{
		return mousePos;
	}

	void setMousePos(const Vec2f& pos)
	{
		mousePos = pos;
	}
};

class Player
{
private:
	// 단위 m
	Vec2f pos{};
	Vec2f dir{};
	float& x{pos.x};
	float& y{pos.y};
	bool show{ false };
	sf::RectangleShape Square{ sf::Vector2f(GRID_WIDTH_PIXEL, GRID_HEIGHT_PIXEL) };
	bool ctrl{ false };

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
		if (not ctrl) {
			return;
		}

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

		

		// 방향 설정
		auto mouse_pos{ WindowToGame(controller.getMousePos()) };
		dir = mouse_pos - pos;
		NormalizeVec(dir);

	}

	void Draw(sf::RenderWindow& window)
	{
		if (not show) return;

		// 윈도우 위치 계산
		auto window_pos{ GameToWindow(pos) };
		auto& windowX = window_pos.x;
		auto& windowY = window_pos.y;

		// 윈도우 위치에 표시
		Square.setPosition(windowX, windowY);
		window.draw(Square);

		if (dir.x == 0.f && dir.y == 0.f) {
			return;
		}

		sf::VertexArray cone(sf::TriangleFan);
		sf::Color cone_color{ 255,0,0,120 };
		cone.append(sf::Vertex(sf::Vector2f{windowX, windowY}, cone_color)); // 중심점

		float angle_degree{ 60.f };
		float angle_rad = angle_degree * 3.14159265f / 180.f;
		float base_angle = std::atan2(-dir.y, dir.x) * 180.f / 3.14159265f;;

		float start_angle = base_angle - angle_degree / 2.f;

		for (int i = 0; i <= 30; ++i) {
			float angle = (start_angle + (angle_degree * i / 30.f)) * 3.14159265f / 180.f;
			sf::Vector2f point = sf::Vector2f{ windowX, windowY }
			+ sf::Vector2f(std::cos(angle), std::sin(angle)) * 50.f;
			cone.append(sf::Vertex(point, cone_color));
		}

		window.draw(cone);
	}


	// getter and setter


	void SetPosition(const Vec2f& _pos) { pos = _pos; }

	void SetPosition(const float _x, const float _y)
	{ SetPosition(Vec2f{ _x, _y }); }

	void SetShow(const bool _show) { show = _show; }

	Vec2f GetPosition() const { return pos; }

	void SetCtrl(const bool _ctrl) { ctrl = _ctrl; }

	void SetDir(const Vec2f& _dir) { dir = _dir; }
	void SetDir(const float _dirx, const float _diry)
	{ dir.x = _dirx; dir.y = _diry; }

	Vec2f GetDir() const { return dir; }

};

class Monster
{
	// 단위 m
	Vec2f pos{};
	Vec2f dir{};
	float& x{ pos.x };
	float& y{ pos.y };
	bool show{ false };
	sf::RectangleShape Square{ sf::Vector2f(GRID_WIDTH_PIXEL, GRID_HEIGHT_PIXEL) };


public:

	Monster(const float x, const float y, const sf::Color color, const bool show) :
		pos{ x, y },
		show{ show }
	{
		SetFillColor(color);
		sf::FloatRect bounds = Square.getLocalBounds();
		Square.setOrigin(bounds.width / 2, bounds.height / 2);
	}

	Monster(const float x, const float y, const sf::Color color) :
		Monster{ x, y, color, false }
	{}

	Monster() :
		Monster{ 0.f, 0.f, sf::Color::Red }
	{}

	void SetFillColor(const sf::Color color)
	{
		Square.setFillColor(color);
	}

	void Update(const Controller& controller, const float delta)
	{

	}

	void Draw(sf::RenderWindow& window)
	{
		if (not show) return;

		// 윈도우 위치 계산
		auto window_pos{ GameToWindow(pos) };
		auto& windowX = window_pos.x;
		auto& windowY = window_pos.y;

		// 윈도우 위치에 표시
		Square.setPosition(windowX, windowY);
		window.draw(Square);

		if (dir.x == 0.f && dir.y == 0.f) {
			return;
		}

		sf::VertexArray cone(sf::TriangleFan);
		sf::Color cone_color{ 255,0,0,120 };
		cone.append(sf::Vertex(sf::Vector2f{ windowX, windowY }, cone_color)); // 중심점

		float angle_degree{ 60.f };
		float angle_rad = angle_degree * 3.14159265f / 180.f;
		float base_angle = std::atan2(-dir.y, dir.x) * 180.f / 3.14159265f;;

		float start_angle = base_angle - angle_degree / 2.f;

		for (int i = 0; i <= 30; ++i) {
			float angle = (start_angle + (angle_degree * i / 30.f)) * 3.14159265f / 180.f;
			sf::Vector2f point = sf::Vector2f{ windowX, windowY }
			+ sf::Vector2f(std::cos(angle), std::sin(angle)) * 50.f;
			cone.append(sf::Vertex(point, cone_color));
		}

		window.draw(cone);
	}


	// getter and setter


	void SetPosition(Vec2f _pos) { pos = _pos; }

	void SetPosition(const float _x, const float _y)
	{
		SetPosition(Vec2f{ _x, _y });
	}

	void SetShow(const bool _show) { show = _show; }

	Vec2f GetPosition() const { return pos; }

	void NormalizeAndSetDir(const Vec2f& _dir) { dir = _dir; }
	void SetDir(const float _dirx, const float _diry)
	{
		dir.x = _dirx; dir.y = _diry;
	}
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
	std::unordered_map<int, Monster> monsters{};
	int my_id = -1;

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
			}
			break;
			case packet::Type::SC_GAME_START:
			{

			}
			break;


			case packet::Type::SC_MATCHMAKING:
			{
				packet::SCMatchmaking packet = reinterpret_cast<packet::SCMatchmaking&>(buffer);
				players[packet.clientId].SetFillColor(sf::Color::Red);
				players[packet.clientId].SetShow(true);
				players[packet.clientId].SetCtrl(true);
				my_id = packet.clientId;
				// TODO: 방 타입 받아와서 설정하기. 지금은 ruins 고정
				// packet.roomType;

				socket_io.DoSend<packet::CSLoadComplete>();
			}

			break;
			case packet::Type::SC_MOVE_PLAYER:
			{
				packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);
				if (not players.contains(packet.clientId)) {
					players[packet.clientId].SetFillColor(sf::Color::Black);
					players[packet.clientId].SetShow(true);
				}
				players[packet.clientId].SetDir(packet.dirX, packet.dirY);
				players[packet.clientId].SetPosition(packet.x, packet.y);
			}
			break;
			case packet::Type::SC_MOVE_MONSTER:
			{
				packet::SCMoveMonster packet = reinterpret_cast<packet::SCMoveMonster&>(buffer);
				if (not monsters.contains(packet.monsterId)) {
					monsters[packet.monsterId].SetFillColor(sf::Color::Green);
					monsters[packet.monsterId].SetShow(true);
				}
				monsters[packet.monsterId].SetDir(packet.dirX, packet.dirY);
				monsters[packet.monsterId].SetPosition(packet.x, packet.y);
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

		// mouse
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2f mouse = (sf::Vector2f)sf::Mouse::getPosition(window);
			controller.setMousePos(Vec2f{ mouse.x, mouse.y });
		}


		// player
		frame_timer.updateDeltaTime();
		auto delta = frame_timer.getDeltaTimeSeconds();


		if (-1 != my_id) {
			players[my_id].Update(controller, delta);

			// send
			if (send_timer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
				send_timer.updateDeltaTime();
				auto pos = players[my_id].GetPosition();
				auto dir = players[my_id].GetDir();
				socket_io.DoSend<packet::CSMovePlayer>(pos.x, pos.y, dir.x, dir.y);
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

		for (auto& [_, monster] : monsters) {
			monster.Draw(window);
		}


		window.display();
	}

	return 0;
}