#pragma once
#include "Object.h"

enum class PlayerType : unsigned char
{
	None = 0,
	Dealer,
	Tanker,
	Healer
};

// ���߿� ��� ������ ����
class Player : public Object
{
public:
	// getter and setter
	void SetPlayerType(const PlayerType& type) { _type = type; }

private:
	// Todo:: Dealer�� �����ϱ�
	PlayerType	_type{ PlayerType::None };
	
};

