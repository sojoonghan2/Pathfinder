#pragma once

enum class PlayerType : unsigned char
{
	None = 0,
	Dealer,
	Tanker,
	Healer
};

// 나중에 상속 구조로 변경
class Player
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	Vec2f GetPos() const { return _pos; }

private:
	Vec2f		_pos{};
	float&		_x{ _pos.x };
	float&		_y{ _pos.y };

	PlayerType	_type{ PlayerType::None };
	
};

