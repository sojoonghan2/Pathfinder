#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Buffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	_boneFinalMatrix = make_shared<Buffer>();
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	if (!_isPlaying) return;
	_updateTime += DELTA_TIME;

	const AnimClipInfo& animClip = _animClips->at(_clipIndex);

	if (_updateTime >= animClip.duration)
		_updateTime = 0.f;

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	_frame = static_cast<int32>(_updateTime * ratio);
	_frame = min(_frame, animClip.frameCount - 1);
	_nextFrame = (_frame + 1) % animClip.frameCount;

	float currentTime = _frame / static_cast<float>(ratio);
	float nextTime = _nextFrame / static_cast<float>(ratio);
	_frameRatio = (_updateTime - currentTime) / (nextTime - currentTime);
	_frameRatio = clamp(_frameRatio, 0.0f, 1.0f);

	// �θ�-�ڽ� ���踦 ����� ���� Ʈ������ ���
	vector<Matrix> finalTransforms(_bones->size());

	for (size_t i = 0; i < _bones->size(); ++i) {
		const BoneInfo& bone = _bones->at(i);

		// ����� ���� �������� Ű������ ������ ��������
		if (i < animClip.keyFrames.size() &&
			_frame < animClip.keyFrames[i].size() &&
			_nextFrame < animClip.keyFrames[i].size())
		{
			const KeyFrameInfo& currentKeyFrame = animClip.keyFrames[i][_frame];
			const KeyFrameInfo& nextKeyFrame = animClip.keyFrames[i][_nextFrame];

			// ������, ȸ��, �̵� ������ ����
			Vec3 interpolatedScale = Vec3::Lerp(currentKeyFrame.scale, nextKeyFrame.scale, _frameRatio);
			Quaternion interpolatedRotation = Quaternion::Slerp(currentKeyFrame.rotation, nextKeyFrame.rotation, _frameRatio);
			Vec3 interpolatedTranslation = Vec3::Lerp(currentKeyFrame.translate, nextKeyFrame.translate, _frameRatio);

			// ������ Ʈ���������� ��� ����
			Matrix blendedTransform = Matrix::CreateScale(interpolatedScale) *
				Matrix::CreateFromQuaternion(interpolatedRotation) *
				Matrix::CreateTranslation(interpolatedTranslation);

			// �θ��� ���� Ʈ������ ����
			if (bone.parentIndex >= 0) {
				blendedTransform = finalTransforms[bone.parentIndex] * blendedTransform;
			}

			// �� ������ ����
			blendedTransform = blendedTransform * bone.matOffset;

			// ���� Ʈ������ ����
			finalTransforms[i] = blendedTransform;
		}
	}

	// GPU ������ ������Ʈ (StructuredBuffer�� ���ε�)
	if (_boneFinalMatrix->GetElementCount() < finalTransforms.size()) {
		_boneFinalMatrix->Init(sizeof(Matrix), finalTransforms.size());
	}
	_boneFinalMatrix->Update(finalTransforms.data(), finalTransforms.size() * sizeof(Matrix));
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	if (!animClips || animClips->empty()) {
		std::cerr << "Error: animClips is nullptr or empty!" << std::endl;
		return;
	}
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

// �÷��� ����
void Animator::Play(uint32 idx)
{
	if (!_animClips || _animClips->empty()) {
		std::cerr << "Error: _animClips is nullptr or empty!" << std::endl;
		return;
	}
	if (idx >= _animClips->size()) {
		std::cerr << "Error: idx (" << idx << ") is out of range!" << std::endl;
		return;
	}

	_clipIndex = idx;
	_updateTime = 0.f;
	_isPlaying = true;
}

void Animator::Stop()
{
	_isPlaying = false;
}

bool Animator::IsAnimationFinished()
{
	if (!_animClips || _clipIndex >= _animClips->size())
		return false;

	const AnimClipInfo& animClip = _animClips->at(_clipIndex);
	return _updateTime >= animClip.duration;
}