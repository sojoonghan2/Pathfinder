#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"
#include "Frustum.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;
Matrix Camera::S_MatShadowView[SHADOWMAP_COUNT];
Matrix Camera::S_MatShadowProjection[SHADOWMAP_COUNT];


Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	_width = static_cast<float>(GEngine->GetWindow().width);
	_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	if (_type == PROJECTION_TYPE::PERSPECTIVE){
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
		Matrix matP = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far * 0.1f);
		_frustum.FinalUpdate(_matView, matP);
	}
	else {
		_matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);
		_frustum.FinalUpdate(_matView, _matProjection);
	}
}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.push_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.push_back(gameObject);
				break;
			}
		}
		if (gameObject->GetParticleSystem()) {
			_vecParticle.push_back(gameObject);
		}
	}
}

void Camera::SortShadowObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecShadow.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		if (!gameObject->IsStatic())
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;
		if (gameObject->GetCheckFrustum()) {
			if (_frustum.ContainsSphereShadow(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}
		

		_vecShadow.push_back(gameObject);
	}
}

// G버퍼에 장면 데이터를 저장한 뒤, 조명을 계산하여 최종 결과를 출력
// 대부분의 불투명한 객체
void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecDeferred);
}

// 각 픽셀마다 조명을 계산
// 투명한 객체, 특수 효과, 조명이 없는 객체
void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecForward);

	for (auto& gameObject : _vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}
}

void Camera::Render_Shadow()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecShadow);

	/*for (auto& gameObject : _vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}*/
}

void Camera::CalculateShadowMatrix()
{
	constexpr array<float, SHADOWMAP_COUNT + 1> cascade = { 0.0f, 0.02f, 0.1f, 0.4f, 1.0f };
	vector<Vec3> frustum = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetFrustum().GetFrustum();
	if (frustum.size() != 8) {
		return;
	}

	for (int i = 0; i < cascade.size() - 1; ++i)
	{
		Vec3 wFrustum[8]{};
		for (int j = 0; j < 8; ++j)
		{
			wFrustum[j] = frustum[j];
		}
		for (int j = 0; j < 4; ++j)
		{
			Vec3 v{ wFrustum[j + 4] - wFrustum[j] };
			Vec3 n{ v * cascade[i] };
			Vec3 f{ v * cascade[i + 1] };

			wFrustum[j + 4] = wFrustum[j] + f;
			wFrustum[j] += n;
		}

		Vec3 center{};
		for (const auto& v : wFrustum)
		{
			center += v;
		}
		center /= 8;

		float radius = 0;
		for (const auto& v : wFrustum)
		{
			float dist = (v - center).Length();
			radius = max(radius, dist);
		}

		float offset{ max(5000.0f, radius) };
		Vec3 shadowLightPos{ center + GetTransform()->GetLook() * -offset };

		Matrix view = ::XMMatrixLookAtLH(shadowLightPos, center, Vec3(0, 1, 0));
		Matrix proj = ::XMMatrixOrthographicLH(radius * 2, radius * 2, 0.0f, 30000);

		Camera::S_MatShadowView[i] = view;
		Camera::S_MatShadowProjection[i] = proj;

	}
}