#include "pch.h"
#include "CrapScript.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

// 이동하는 거

void CrapScript::Update()
{

	bool isMoving = false;

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		SetAnimationState(AnimationState::Walk);
		isMoving = true;
		Vec3 pos = GetTransform()->GetLocalPosition();
		// * 애니메이션 존재할 경우 작동
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

		GetTransform()->SetLocalPosition(pos);
	} 
	else if (INPUT->GetButton(KEY_TYPE::C))
	{
		cout << "이동중" << endl;
		SetAnimationState(AnimationState::Walk);
		isMoving = true;
		Vec3 pos = GetTransform()->GetLocalPosition();
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		anicount = (currentIndex + 1) % count;
		GetAnimator()->Play(anicount);
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;   

		GetTransform()->SetLocalPosition(pos);
	}

	if (!isMoving) {
		cout << "이동 없음 → Idle 상태로 변경" << endl;
		SetAnimationState(AnimationState::Idle);
	}

}

// 여기부터
void CrapScript::SetAnimationState(AnimationState state)
{
	if (_currentState == state) {
		return;
	}

	_currentState = state;
	shared_ptr<Animator> animator = GetAnimator();

	// FBX파일에 애니메이션이 여러개 저장되어 있는 경우 번호에 따라 다르게 표시된다
	if (state == AnimationState::Idle)
	{
		cout << "애니메이션 변경" << endl;
		animator->Play(0);
	}
	else if (state == AnimationState::Walk)
	{
		cout << "애니메이션 변경" << endl;
		animator->Play(1);
	}

}