#include "pch.h"
#include "CrapScript.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

// �̵��ϴ� ��

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

// �������
void CrapScript::SetAnimationState(AnimationState state)
{
	if (_currentState == state) {
		return;
	}

	_currentState = state;
	shared_ptr<Animator> animator = GetAnimator();

	// FBX���Ͽ� �ִϸ��̼��� ������ ����Ǿ� �ִ� ��� ��ȣ�� ���� �ٸ��� ǥ�õȴ�
	if (state == AnimationState::Idle)
	{
		cout << "�ִϸ��̼� ����" << endl;
		animator->Play(0);
	}
	else if (state == AnimationState::Walk)
	{
		cout << "�ִϸ��̼� ����" << endl;
		animator->Play(1);
	}

}