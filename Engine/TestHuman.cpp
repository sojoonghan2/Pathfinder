#include "pch.h"
#include "TestHuman.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

// �̵��ϴ� ��

void TestHuman::Update()
{
	bool isMoving = false;
	if (GetAnimator() == nullptr) {
		cout << "��" << endl;
	}
	//������� ����

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
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

		GetTransform()->SetLocalPosition(pos);
	}

	if (!isMoving) {
		cout << "�̵� ���� �� Idle ���·� ����" << endl;
		SetAnimationState(AnimationState::Idle);
	}
}

// �������
void TestHuman::SetAnimationState(AnimationState state)
{
	if (_currentState == state) {
		return;
	}

	_currentState = state;
	shared_ptr<Animator> animator = GetAnimator();
	//int32 count = GetAnimator()->GetAnimCount();
	//int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
	//anicount = (currentIndex + 1) % count;
	//GetAnimator()->Play(anicount);
	// FBX���Ͽ� �ִϸ��̼��� ������ ����Ǿ� �ִ� ��� ��ȣ�� ���� �ٸ��� ǥ�õȴ�
	if (state == AnimationState::Idle)
	{
		cout << "�ִϸ��̼� ����" << endl;
		animator->Play(1);
	}
	else if (state == AnimationState::Walk)
	{
		cout << "�ִϸ��̼� ����" << endl;
		animator->Play(3);
	}
}