#pragma once
#include "Object.h"

enum class PlayerType : unsigned char
{
	None = 0,
	Dealer,
	Tanker,
	Healer
};

// 나중에 상속 구조로 변경
class Player : public Object
{
public:
	// getter and setter
	void SetPlayerType(const PlayerType& type) { _type = type; }

private:
	// Todo:: Dealer로 변경하기
	PlayerType	_type{ PlayerType::None };
	
};

