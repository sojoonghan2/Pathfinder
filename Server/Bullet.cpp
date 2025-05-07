#include "pch.h"
#include "Bullet.h"
#include "SweptCollider.h"


void Bullet::Update(const float delta)
{
	_prevPos = GetPos();
	Move(delta);
}

void Bullet::InitBullet(const Vec2f& pos)
{
	SetPos(pos);
	_prevPos = pos;
	SetCollider(std::make_shared<SweptCollider>(GetPosRef(), _prevPos));
	SetSpeed(BULLET_SPEED_MPS);
}
