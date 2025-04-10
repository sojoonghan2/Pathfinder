#pragma once
#include "Mesh.h"
#include "Material.h"

class DebugRenderer
{
public:
    static void Init();
    static void DrawSphere(const Vec3& center, float radius, const Vec4& color);
    static void Render();

private:
    struct DebugDrawCommand
    {
        enum class Type { SPHERE } type;
        Vec3 center;
        float radius;
        Vec4 color;
    };

    static std::vector<DebugDrawCommand> _commands;
    static std::shared_ptr<Mesh> _sphereMesh;
    static std::shared_ptr<Material> _debugMaterial;
};