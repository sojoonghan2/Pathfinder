#include "pch.h"
#include "TestHuman.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"


void TestHuman::Update()
{

	Vec3 pos = GetTransform()->GetLocalPosition();

	int32 count = GetAnimator()->GetAnimCount();
	int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
	anicount = (currentIndex + 1) % count;
	GetAnimator()->Play(anicount);

	if (INPUT->GetButton(KEY_TYPE::Z))
	{

		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
	} 
	else if (INPUT->GetButton(KEY_TYPE::C))
	{

		pos -= Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
	}

	GetTransform()->SetLocalPosition(pos);


}