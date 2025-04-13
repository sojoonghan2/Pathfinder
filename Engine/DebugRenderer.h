#pragma once
#include "Mesh.h"
#include "Material.h"

class DebugRenderer
{
	DECLARE_SINGLE(DebugRenderer);

public:
	static void Init();
	static void Render();

	static void DrawSphere(const Vec3& center, float radius, const Vec4& color);

private:
	struct DebugDrawCommand
	{
		enum class Type { SPHERE };
		Type type;
		Vec3 center;
		float radius;
		Vec4 color;
	};

	static std::vector<DebugDrawCommand> _commands;
	static std::shared_ptr<Mesh> _sphereMesh;
	static std::shared_ptr<Material> _debugMaterial;
};
