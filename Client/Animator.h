#pragma once
#include "Component.h"
#include "Mesh.h"

class Material;
class Buffer;
class Mesh;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

public:
	void SetBones(const vector<BoneInfo>* bones) { _bones = bones; }
	void SetAnimClip(const vector<AnimClipInfo>* animClips);
	void PushData();

	int32 GetAnimCount() { return static_cast<uint32>(_animClips->size()); }
	int32 GetCurrentClipIndex() { return _clipIndex; }
	void Play(uint32 idx);
	void Stop();

	bool IsAnimationFinished();

public:
	virtual void FinalUpdate() override;

private:
	const vector<BoneInfo>* _bones;
	const vector<AnimClipInfo>* _animClips;

	float							_updateTime = 0.f;
	int32							_clipIndex = 0;
	int32							_frame = 0;
	int32							_nextFrame = 0;
	float							_frameRatio = 0;
	shared_ptr<Material>			_computeMaterial;
	shared_ptr<Buffer>	_boneFinalMatrix;  // Ư�� �������� ���� ���
	bool							_boneFinalUpdated = false;
	bool							_isPlaying = true;
};
