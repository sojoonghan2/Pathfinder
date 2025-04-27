#pragma once

enum class ObjectType : unsigned char
{
	None,
	Player,
	Monster,
	Bullet,
};

class Object
{
public:
	void Move(const float delta_time);


	// getter and setter
	Vec2f GetPos() const { return _pos; }
	void SetPos(const Vec2f& pos);
	void SetPos(const float x, const float y);

	Vec2f GetDir() const { return _dir; }
	void SetDir(const Vec2f& dir) { _dir = dir; }

	ObjectType GetObjectType() const { return _objectType; }
	void SetObjectType(const ObjectType object_type) { _objectType = object_type; }

	void SetSpeed(const float speed) { _speed = speed; }
	float GetSpeed() const { return _speed; }


	Object() = default;
	Object(const ObjectType object_type);

private:
	Vec2f		_pos{};
	Vec2f		_dir{};
	float		_speed{ 0.f };


	ObjectType	_objectType{ ObjectType::None };
};

