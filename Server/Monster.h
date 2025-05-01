#pragma once
#include "Object.h"
#include "AABBCollider.h"


enum class MonsterType : unsigned char
{
	None = 0,
	Crab,
	Tank,
	Robot,
	Core
};

// ���߿� ��� ������ ����
class Monster : public Object
{
public:
	//bool GetRunning() const;

	//// CAS�� ����ؼ� ����.
	//bool TrySetRunning(const bool running);

	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::Monster; }

	void SetMonsterType(const MonsterType type) { _monsterType = type; }
	
	void SetRoomId(const int room_id) { _roomId = room_id; }	
	int GetRoomId() const { return _roomId; }

	// �ڵ������� Collider ����.
	// Todo: 
	// �ϴ� �̷��� �ϴµ� ���߿� SETMONSTERTYPE���� �ٲ�� �ؾ��Ѵ�.
	Monster()
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
	}

private:
	int		_roomId{ -1 };

	MonsterType			_monsterType{ MonsterType::None };
	
};

