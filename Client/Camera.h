#pragma once
#include "Component.h"
#include "Frustum.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE, // 원근 투영
	ORTHOGRAPHIC, // 직교 투영
};

// 씬 내의 시점을 정의하고, 뷰/투영 행렬 계산 및 렌더링 대상 오브젝트를 분류하는 카메라
// 디퍼드, 포워드, 그림자 렌더링 방식을 지원하며, 레이어 마스크 기반으로 렌더링
class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;

	void SetProjectionType(PROJECTION_TYPE type) { _type = type; }
	PROJECTION_TYPE GetProjectionType() { return _type; }

	void SortGameObject();
	void SortShadowObject();

	void Render_Deferred();
	void Render_Forward();
	void Render_Shadow();

	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			_cullingMask |= (1 << layer);
		else
			_cullingMask &= ~(1 << layer);
	}

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
	void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	bool IsCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }

	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }
	void SetScale(float value) { _scale = value; }
	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	Matrix& GetViewMatrix() { return _matView; }
	Matrix& GetProjectionMatrix() { return _matProjection; }

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.f;
	float _far = 2000.f;
	float _fov = XM_PI / 2.f;
	float _scale = 1.f;
	float _width = 0.f;
	float _height = 0.f;

	Matrix _matView = {};
	Matrix _matProjection = {};

	Frustum _frustum;
	uint32 _cullingMask = 0;

private:
	vector<shared_ptr<GameObject>>	_vecDeferred;
	vector<shared_ptr<GameObject>>	_vecForward;
	vector<shared_ptr<GameObject>>	_vecParticle;
	vector<shared_ptr<GameObject>>	_vecShadow;

public:
	// TEMP
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};
