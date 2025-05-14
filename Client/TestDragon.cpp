#include "pch.h"
#include "TestDragon.h"
#include "Input.h"
#include "Amature.h"

void TestDragon::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		int32 count = GetAmature()->GetAnimCount();
		int32 currentIndex = GetAmature()->GetCurrentClipIndex();

		int32 index = (currentIndex + 1) % count;
		GetAmature()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAmature()->GetAnimCount();
		int32 currentIndex = GetAmature()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAmature()->Play(index);
	}
}