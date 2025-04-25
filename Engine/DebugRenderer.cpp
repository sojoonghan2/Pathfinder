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
    // X축 (빨간색)
    DrawLine(origin, origin + Vec3(size, 0, 0), Vec4(1, 0, 0, 1));
    // Y축 (녹색)
    DrawLine(origin, origin + Vec3(0, size, 0), Vec4(0, 1, 0, 1));
    // Z축 (파란색)
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

        // 명령 유형에 따라 적절한 변환 행렬 구성
        switch (cmd.type)
        {
        case DebugDrawCommand::Type::SPHERE:
        {
            meshToUse = _sphereMesh;

            // 구의 위치와 크기 출력
            std::cout << "Rendering Sphere - Center: (" << cmd.center.x << ", " << cmd.center.y << ", " << cmd.center.z
                << "), Radius: " << cmd.radius << std::endl;

            // DirectX에서 행렬 곱셈 순서는 왼쪽에서 오른쪽으로
            // 스케일링 행렬 (S)
            XMMATRIX S = XMMatrixScaling(cmd.radius, cmd.radius, cmd.radius);

            // 이동 행렬 (T)
            XMMATRIX T = XMMatrixTranslation(cmd.center.x, cmd.center.y, cmd.center.z);

            // 최종 월드 변환 행렬 W = S * T (먼저 스케일링 후 이동)
            W = S * T;
            break;
        }
        default:
            continue; // 알 수 없는 명령 유형은 무시
        }

        if (meshToUse == nullptr)
            continue;

        // WVP 행렬 구성
        TransformParams t = {};
        t.matWorld = XMMatrixTranspose(W);
        t.matView = XMMatrixTranspose(V);
        t.matProjection = XMMatrixTranspose(P);
        t.matWV = XMMatrixTranspose(W * V);
        t.matWVP = XMMatrixTranspose(W * V * P);

        // 뷰 역행렬 계산
        XMVECTOR det;
        t.matViewInv = XMMatrixTranspose(XMMatrixInverse(&det, V));

        // 상수 버퍼에 변환 행렬 전달
        CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&t, sizeof(t));

        // 재질 파라미터 설정 (색상 포함)
        MaterialParams m = {};
        CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&m, sizeof(m));

        // 그래픽스 디스크립터 힙 커밋
        GEngine->GetGraphicsDescHeap()->CommitTable();

        // 머티리얼 데이터 푸시 및 렌더링
        _debugMaterial->PushGraphicsData();
        meshToUse->Render();
    }

    // 렌더링 후 명령 버퍼 비우기
    _commands.clear();
}
*/