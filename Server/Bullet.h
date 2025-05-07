#pragma once
#include "Object.h"

class Bullet : public Object
{
public:
	ObjectType GetObjectType() const override { return ObjectType::Bullet; }

	void Update(const float delta) override;
	void InitBullet(const Vec2f& pos);

private:
	Vec2f _prevPos{};
};

