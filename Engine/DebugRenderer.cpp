#include "pch.h"
#include "DebugRenderer.h"
#include "Engine.h"
#include "Resources.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

std::vector<DebugRenderer::DebugDrawCommand> DebugRenderer::_commands;
std::shared_ptr<Mesh> DebugRenderer::_sphereMesh;
std::shared_ptr<Material> DebugRenderer::_debugMaterial;

void DebugRenderer::Init()
{
	_sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	_debugMaterial = GET_SINGLE(Resources)->Get<Material>(L"Debug");
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
		if (cmd.type == DebugDrawCommand::Type::SPHERE)
		{
			const Matrix S = Matrix::CreateScale(cmd.radius, cmd.radius, cmd.radius);
			const Matrix T = Matrix::CreateTranslation(cmd.center);
			const Matrix W = S * T;

			// ����� ��� (�������� �󸶳� �ָ� �ִ��� Ȯ��)
			Vec3 eyeSpacePos = Vec3::Transform(cmd.center, V);
			std::cout << "Eye space center: (" << eyeSpacePos.x << ", " << eyeSpacePos.y << ", " << eyeSpacePos.z << ")\n";

			// WVP ����
			TransformParams t = {};
			t.matWorld = XMMatrixTranspose(W);
			t.matView = XMMatrixTranspose(V);
			t.matProjection = XMMatrixTranspose(P);
			t.matWV = XMMatrixTranspose(W * V);
			t.matWVP = XMMatrixTranspose(W * V * P);
			t.matViewInv = XMMatrixTranspose(XMMatrixInverse(nullptr, V));

			CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&t, sizeof(t));

			// ���� ����
			MaterialParams m = {};
			CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&m, sizeof(m));

			// Ŀ�� �� �׸���
			GEngine->GetGraphicsDescHeap()->CommitTable();
			_debugMaterial->PushGraphicsData();
			_sphereMesh->Render();
		}
	}

	_commands.clear();
}
