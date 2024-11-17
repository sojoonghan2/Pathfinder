#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	_boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	_updateTime += DELTA_TIME;

	// 몇 번째 클립?
	const AnimClipInfo& animClip = _animClips->at(_clipIndex);
	if (_updateTime >= animClip.duration)
		_updateTime = 0.f;

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	// 현재 프레임
	_frame = static_cast<int32>(_updateTime * ratio);
	_frame = min(_frame, animClip.frameCount - 1);
	// 다음 프레임
	_nextFrame = min(_frame + 1, animClip.frameCount - 1);
	// 1과 2프레임 사이에 있는 애매한 1.6프레임 같은 애들을 frameRatio라고 함
	_frameRatio = static_cast<float>(_frame - _frame);
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	_animClips = animClips;
}

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(_bones->size());
	if (_boneFinalMatrix->GetElementCount() < boneCount)
		_boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	_computeMaterial->SetInt(0, boneCount);
	_computeMaterial->SetInt(1, _frame);
	_computeMaterial->SetInt(2, _nextFrame);
	_computeMaterial->SetFloat(0, _frameRatio);

	uint32 groupCount = (boneCount / 256) + 1;
	_computeMaterial->Dispatch(groupCount, 1, 1);

	_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx)
{
	assert(idx < _animClips->size());
	_clipIndex = idx;
	_updateTime = 0.f;
}