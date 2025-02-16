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

    // 부모-자식 관계를 고려한 최종 트랜스폼 계산
    vector<Matrix> finalTransforms(_bones->size());

    for (size_t i = 0; i < _bones->size(); ++i) {
        const BoneInfo& bone = _bones->at(i);

        // 현재와 다음 프레임의 키프레임 데이터 가져오기
        const KeyFrameInfo& currentKeyFrame = animClip.keyFrames[i][_frame];
        const KeyFrameInfo& nextKeyFrame = animClip.keyFrames[i][_nextFrame];

        // 스케일, 회전, 이동 데이터 보간
        Vec3 interpolatedScale = MyProject::Lerp(currentKeyFrame.scale, nextKeyFrame.scale, _frameRatio);
        Quaternion interpolatedRotation = MyProject::QuaternionSlerp(currentKeyFrame.rotation, nextKeyFrame.rotation, _frameRatio);
        Vec3 interpolatedTranslation = MyProject::Lerp(currentKeyFrame.translate, nextKeyFrame.translate, _frameRatio);

        // 보간된 트랜스폼으로 행렬 생성
        Matrix blendedTransform = MyProject::MatrixAffineTransformation(interpolatedScale, interpolatedRotation, interpolatedTranslation);

        // 부모의 최종 트랜스폼 적용
        if (bone.parentIndex >= 0) {
            blendedTransform = finalTransforms[bone.parentIndex] * blendedTransform;
        }

        // 본 오프셋 적용
        blendedTransform = blendedTransform * bone.matOffset;

        // 최종 트랜스폼 저장
        finalTransforms[i] = blendedTransform;
    }

    // GPU 데이터 업데이트 (StructuredBuffer에 업로드)
    if (_boneFinalMatrix->GetElementCount() < finalTransforms.size()) {
        _boneFinalMatrix->Init(sizeof(Matrix), finalTransforms.size());
    }
    _boneFinalMatrix->Update(finalTransforms.data(), finalTransforms.size() * sizeof(Matrix));
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

// 플레이 수정
void Animator::Play(uint32 idx)
{
	assert(idx < _animClips->size());
    cout << _animClips->size() << endl;
	_clipIndex = idx;
	_updateTime = 0.f;
}
