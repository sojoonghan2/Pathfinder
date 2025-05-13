#include "pch.h"
#include "AABBCollider.h"
#include "CircleCollider.h"
#include "SweptCollider.h"


bool AABBCollider::CheckCollision(const std::shared_ptr<const Collider>& other) const
{
	return other->CheckCollisionWithAABB(*this);
}

bool AABBCollider::CheckCollisionWithAABB(const AABBCollider& other) const
{
	Vec2f a_min{ GetMin() };
	Vec2f a_max{ GetMax() };
	Vec2f b_min{ other.GetMin() };
	Vec2f b_max{ other.GetMax() };

	return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
		(a_min.y <= b_max.y && a_max.y >= b_min.y);
}

bool AABBCollider::CheckCollisionWithCircle(const CircleCollider& other) const
{

	Vec2f min{ GetMin() };
	Vec2f max{ GetMax() };

	float closest_x = std::max(min.x, std::min(other.center.x, max.x));
	float closest_y = std::max(min.y, std::min(other.center.y, max.y));

	Vec2f diff = other.center - Vec2f{closest_x, closest_y};
	return diff.x * diff.x + diff.y * diff.y <= other.radius * other.radius;
}

bool AABBCollider::CheckCollisionWithSwept(const SweptCollider& other) const
{

	Vec2f rect_min{ GetMin() };
	Vec2f rect_max{ GetMax() };
	Vec2f line_start{ other._prevPos };
	Vec2f line_end{ other._pos };

	// 선분의 AABB 구하기
	float line_min_x{ std::min(line_start.x, line_end.x) };
	float line_max_x{ std::max(line_start.x, line_end.x) };
	float line_min_y{ std::min(line_start.y, line_end.y) };
	float line_max_y{ std::max(line_start.y, line_end.y) };

	// 빠른 AABB 체크
	if (line_max_x < rect_min.x || line_min_x > rect_max.x ||
		line_max_y < rect_min.y || line_min_y > rect_max.y) {
		return false;
	}

	// 선분의 끝점이 사각형 안에 있는지 검사
	if (line_start.x >= rect_min.x && line_start.x <= rect_max.x &&
		line_start.y >= rect_min.y && line_start.y <= rect_max.y) {
		return true;
	}

	if (line_end.x >= rect_min.x && line_end.x <= rect_max.x &&
		line_end.y >= rect_min.y && line_end.y <= rect_max.y) {
		return true;
	}

	// 사각형의 네 변 정의
	std::array<Vec2f, 4> corners{
		Vec2f{ rect_min.x, rect_min.y },
		Vec2f{ rect_max.x, rect_min.y },
		Vec2f{ rect_max.x, rect_max.y },
		Vec2f{ rect_min.x, rect_max.y }
	};

	auto IsLineIntersect = [](const Vec2f& p1, const Vec2f& p2, const Vec2f& q1, const Vec2f& q2) -> bool
	{
		auto CCW = [](const Vec2f& a, const Vec2f& b, const Vec2f& c) -> bool
		{
			return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
		};

		return CCW(p1, q1, q2) != CCW(p2, q1, q2) && CCW(p1, p2, q1) != CCW(p1, p2, q2);
	};

	// 선분과 사각형 변들의 교차 검사
	for (int i{ 0 }; i < 4; ++i) {
		Vec2f edge_start{ corners[i] };
		Vec2f edge_end{ corners[(i + 1) % 4] };

		if (IsLineIntersect(line_start, line_end, edge_start, edge_end)) {
			return true;
		}
	}

	return false;
}

void AABBCollider::ResolveCollisionWithAABB(AABBCollider& other)
{
	float dx = _center.x - other._center.x;
	float px = (_width + other._width) / 2.f - std::abs(dx);
	if (px <= 0) return;

	float dy = _center.y - other._center.y;
	float py = (_height + other._height) / 2.f - std::abs(dy);
	if (py <= 0) return;

	// 최소 침투 방향으로 밀어냄
	if (px < py)
	{
		float sx = (dx < 0) ? -1.0f : 1.0f;
		float move = px / 2.0f;
		_center.x -= move * sx;
		other._center.x += move * sx;
	}
	else
	{
		float sy = (dy < 0) ? -1.0f : 1.0f;
		float move = py / 2.0f;
		_center.y -= move * sy;
		other._center.y += move * sy;
	}
}


