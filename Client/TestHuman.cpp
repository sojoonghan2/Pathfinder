#include "pch.h"
#include "TestHuman.h"
#include "Input.h"
#include "Timer.h"
#include "Amature.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

// �̵��ϴ� ��

void TestHuman::Update()
{
	bool isMoving = false;
	if (GetAmature() == nullptr) {
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
	shared_ptr<Amature> amature = GetAmature();
	//int32 count = GetAmature()->GetAnimCount();
	//int32 currentIndex = GetAmature()->GetCurrentClipIndex();
	//anicount = (currentIndex + 1) % count;
	//GetAmature()->Play(anicount);
	// FBX���Ͽ� �ִϸ��̼��� ������ ����Ǿ� �ִ� ��� ��ȣ�� ���� �ٸ��� ǥ�õȴ�
	if (state == AnimationState::Idle)
	{
		cout << "�ִϸ��̼� ����" << endl;
		amature->Play(1);
	}
	else if (state == AnimationState::Walk)
	{
		cout << "�ִϸ��̼� ����" << endl;
		amature->Play(3);
	}
}