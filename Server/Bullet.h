#pragma once
#include "Object.h"

class Bullet : public Object
{
public:
	ObjectType GetObjectType() const override { return ObjectType::BULLET; }

	void Update(const float delta) override;
	void InitBullet(const Vec2f& pos, const Vec2f& dir, const int player_id);

	// getter and setter
	int GetPlayerId() const { return _playerId; }

private:
	Vec2f _prevPos{};
	int   _playerId{ -1 };
};

