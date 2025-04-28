#pragma once
#include "Object.h"
#include "AABBCollider.h"

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
	ObjectType GetObjectType() const override { return ObjectType::Player; }

	void SetPlayerType(const PlayerType& type) { _type = type; }

	Player()
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), PLAYER_SIZE_M, PLAYER_SIZE_M));
	}

private:
	// Todo:: Dealer로 변경하기
	PlayerType	_type{ PlayerType::None };
	
};

