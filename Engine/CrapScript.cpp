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

	bool isMoving = false;

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		SetAnimationState(AnimationState::Walk);
		isMoving = true;
		Vec3 pos = GetTransform()->GetLocalPosition();
		// * �ִϸ��̼� ������ ��� �۵�
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

		GetTransform()->SetLocalPosition(pos);
	} 
	else if (INPUT->GetButton(KEY_TYPE::C))
	{
		cout << "�̵���" << endl;
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
		cout << "�̵� ���� �� Idle ���·� ����" << endl;
		SetAnimationState(AnimationState::Idle);
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