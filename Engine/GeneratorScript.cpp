#include "pch.h"
#include "GeneratorScript.h"
#include "Input.h"
#include "Animator.h"

GeneratorScript::GeneratorScript()
{
}

void GeneratorScript::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::U))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
	}
}