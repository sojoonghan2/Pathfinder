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
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
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