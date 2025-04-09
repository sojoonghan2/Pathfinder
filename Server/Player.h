#pragma once


struct Player
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	void SetClientId(const int id) { _clientId = id; }
	int GetClientId() const { return _clientId; }

	bool GetRunning() const { return _isRunning.load(); }
	bool TrySetRunning(const bool running); 

private:
	Vec2f _pos{};
	int _clientId{ -1 };
	float& _x{ _pos.x };
	float& _y{ _pos.y };

	std::atomic_bool _isRunning{ false };
};