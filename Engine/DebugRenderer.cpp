#include "pch.h"
#include "DebugRenderer.h"
#include "Engine.h"
#include "Resources.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

vector<DebugRenderer::DebugDrawCommand> DebugRenderer::_commands;
shared_ptr<Mesh> DebugRenderer::_sphereMesh;
shared_ptr<Material> DebugRenderer::_debugMaterial;

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

	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	if (camera == nullptr)
		return;

	_debugMaterial->PushGraphicsData();

	for (const auto& cmd : _commands)
	{
		if (cmd.type == DebugDrawCommand::Type::SPHERE)
		{
			Matrix S = Matrix::CreateScale(cmd.radius);
			Matrix T = Matrix::CreateTranslation(cmd.center);
			Matrix W = S * T;

			// TRANSFORM 상수 버퍼 전송
			TransformParams transform = {};
			transform.matWorld = XMMatrixTranspose(W);
			transform.matView = XMMatrixTranspose(camera->GetViewMatrix());
			transform.matProjection = XMMatrixTranspose(camera->GetProjectionMatrix());
			transform.matWV = XMMatrixTranspose(W * camera->GetViewMatrix());
			transform.matWVP = XMMatrixTranspose(W * camera->GetViewMatrix() * camera->GetProjectionMatrix());

			CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transform, sizeof(transform));

			// MATERIAL 상수 버퍼 전송
			MaterialParams material = {};
			//material.matrixParams = cmd.color;

			CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&material, sizeof(material));

			// 구체 렌더링
			_sphereMesh->Render();
		}
	}

	_commands.clear();
}
