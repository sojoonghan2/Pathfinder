#include "pch.h"
#include "Bullet.h"
#include "SweptCollider.h"


void Bullet::Update(const float delta)
{
	_prevPos = GetPos();
	Move(delta);
}

void Bullet::InitBullet(const Vec2f& pos, const Vec2f& dir)
{
	SetPos(pos);
	_prevPos = pos;
	SetCollider(std::make_shared<SweptCollider>(GetPosRef(), _prevPos));
	SetSpeed(PLAYER_BULLET_SPEED_MPS);
	SetDir(dir);
}
