#include "pch.h"
#include "DebugRenderer.h"
#include "Engine.h"
#include "Resources.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

/*
std::vector<DebugRenderer::DebugDrawCommand> DebugRenderer::_commands;
std::shared_ptr<Mesh> DebugRenderer::_sphereMesh;
std::shared_ptr<Mesh> DebugRenderer::_boxMesh;
std::shared_ptr<Mesh> DebugRenderer::_lineMesh;
std::shared_ptr<Material> DebugRenderer::_debugMaterial;

void DebugRenderer::Init()
{
    _sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
    _debugMaterial = GET_SINGLE(Resources)->Get<Material>(L"Debug");
}

void DebugRenderer::Release()
{
    _sphereMesh = nullptr;
    _boxMesh = nullptr;
    _lineMesh = nullptr;
    _debugMaterial = nullptr;
    _commands.clear();
}

void DebugRenderer::DrawSphere(const Vec3& center, float radius, const Vec4& color)
{
    DebugDrawCommand cmd;
    cmd.type = DebugDrawCommand::Type::SPHERE;
    cmd.center = center;
    cmd.radius = radius;
    cmd.color = color;
    _commands.push_back(cmd);
}

void DebugRenderer::DrawLine(const Vec3& start, const Vec3& end, const Vec4& color)
{
    DebugDrawCommand cmd;
    cmd.type = DebugDrawCommand::Type::LINE;
    cmd.center = start;
    cmd.end = end;
    cmd.color = color;
    _commands.push_back(cmd);
}

void DebugRenderer::DrawBox(const Vec3& center, const Vec3& extents, const Vec3& rotation, const Vec4& color)
{
    DebugDrawCommand cmd;
    cmd.type = DebugDrawCommand::Type::BOX;
    cmd.center = center;
    cmd.extents = extents;
    cmd.rotation = rotation;
    cmd.color = color;
    _commands.push_back(cmd);
}

void DebugRenderer::DrawAxes(const Vec3& origin, float size)
{
    // X�� (������)
    DrawLine(origin, origin + Vec3(size, 0, 0), Vec4(1, 0, 0, 1));
    // Y�� (���)
    DrawLine(origin, origin + Vec3(0, size, 0), Vec4(0, 1, 0, 1));
    // Z�� (�Ķ���)
    DrawLine(origin, origin + Vec3(0, 0, size), Vec4(0, 0, 1, 1));
}

void DebugRenderer::Render()
{
    if (_commands.empty())
        return;

    auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
    if (camera == nullptr)
        return;

    const Matrix V = camera->GetViewMatrix();
    const Matrix P = camera->GetProjectionMatrix();

    for (const auto& cmd : _commands)
    {
        std::shared_ptr<Mesh> meshToUse = nullptr;
        Matrix W;

        // ��� ������ ���� ������ ��ȯ ��� ����
        switch (cmd.type)
        {
        case DebugDrawCommand::Type::SPHERE:
        {
            meshToUse = _sphereMesh;

            // ���� ��ġ�� ũ�� ���
            std::cout << "Rendering Sphere - Center: (" << cmd.center.x << ", " << cmd.center.y << ", " << cmd.center.z
                << "), Radius: " << cmd.radius << std::endl;

            // DirectX���� ��� ���� ������ ���ʿ��� ����������
            // �����ϸ� ��� (S)
            XMMATRIX S = XMMatrixScaling(cmd.radius, cmd.radius, cmd.radius);

            // �̵� ��� (T)
            XMMATRIX T = XMMatrixTranslation(cmd.center.x, cmd.center.y, cmd.center.z);

            // ���� ���� ��ȯ ��� W = S * T (���� �����ϸ� �� �̵�)
            W = S * T;
            break;
        }
        default:
            continue; // �� �� ���� ��� ������ ����
        }

        if (meshToUse == nullptr)
            continue;

        // WVP ��� ����
        TransformParams t = {};
        t.matWorld = XMMatrixTranspose(W);
        t.matView = XMMatrixTranspose(V);
        t.matProjection = XMMatrixTranspose(P);
        t.matWV = XMMatrixTranspose(W * V);
        t.matWVP = XMMatrixTranspose(W * V * P);

        // �� ����� ���
        XMVECTOR det;
        t.matViewInv = XMMatrixTranspose(XMMatrixInverse(&det, V));

        // ��� ���ۿ� ��ȯ ��� ����
        CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&t, sizeof(t));

        // ���� �Ķ���� ���� (���� ����)
        MaterialParams m = {};
        CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&m, sizeof(m));

        // �׷��Ƚ� ��ũ���� �� Ŀ��
        GEngine->GetGraphicsDescHeap()->CommitTable();

        // ��Ƽ���� ������ Ǫ�� �� ������
        _debugMaterial->PushGraphicsData();
        meshToUse->Render();
    }

    // ������ �� ��� ���� ����
    _commands.clear();
}
*/