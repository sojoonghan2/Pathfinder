#pragma once
#include "Collider.h"

class Object
{
public:
	void Move(const float delta_time);


	// getter and setter
	virtual ObjectType GetObjectType() const = 0;

	Vec2f GetPos() const { return _pos; }
	void SetPos(const Vec2f& pos);
	void SetPos(const float x, const float y);

	Vec2f GetDir() const { return _dir; }
	void SetDir(const Vec2f& dir) { _dir = dir; }

	void SetSpeed(const float speed) { _speed = speed; }
	float GetSpeed() const { return _speed; }

	void SetCollider(std::shared_ptr<Collider>&& collider)
		{ _collider = std::move(collider); }

	// collider 생성을 위한 레퍼런스 getter
	const Vec2f& GetPosRef() const { return _pos; }

	Object() = default;

	// 복사 금지.
	Object(const Object& other) = delete;
	Object& operator=(const Object& other) = delete;
	

public:
	Vec2f		_pos{};
	Vec2f		_dir{};
	float		_speed{ 0.f };
	std::shared_ptr<Collider> _collider{ nullptr };

};

