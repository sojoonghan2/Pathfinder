#pragma once
#include "Mesh.h"
#include "Material.h"

/*
class DebugRenderer
{
	DECLARE_SINGLE(DebugRenderer);

public:
    struct DebugDrawCommand
    {
        enum class Type
        {
            SPHERE,
            LINE,
            BOX,
            AXIS
        };

        Type type;
        Vec3 center;
        float radius;
        Vec4 color;

        // ���� ������� ���� �߰� �ʵ�
        Vec3 end;

        // �ڽ� ������� ���� �߰� �ʵ�
        Vec3 extents;
        Vec3 rotation;
    };

public:
    static void Init();
    static void Render();
    static void Release();

    // ����� ����� �Լ���
    static void DrawSphere(const Vec3& center, float radius, const Vec4& color = Vec4(0, 1, 0, 1));
    static void DrawLine(const Vec3& start, const Vec3& end, const Vec4& color = Vec4(1, 1, 1, 1));
    static void DrawBox(const Vec3& center, const Vec3& extents, const Vec3& rotation = Vec3(0, 0, 0), const Vec4& color = Vec4(0, 1, 0, 1));
    static void DrawAxes(const Vec3& origin, float size = 10.0f);

private:
    static std::vector<DebugDrawCommand> _commands;
    static std::shared_ptr<Mesh> _sphereMesh;
    static std::shared_ptr<Mesh> _boxMesh;
    static std::shared_ptr<Mesh> _lineMesh;
    static std::shared_ptr<Material> _debugMaterial;
};
*/