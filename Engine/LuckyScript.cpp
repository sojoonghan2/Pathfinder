#include "pch.h"
#include "LuckyScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"

LuckyScript::LuckyScript()
{
}

LuckyScript::~LuckyScript() {}

void LuckyScript::LateUpdate()
{
	_treasureChest = GET_SINGLE(SceneManager)->FindObjectByName(L"TreasureChest2");

	KeyboardInput();
	MouseInput();

	// 게임 윈도우가 포커스 되었을 때만 입력 처리
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
			ApplyModuleAbillities();
			GET_SINGLE(SceneManager)->LoadScene(L"BossScene");
		}
	}

	if (_getChest)
	{
		Vec3 currentPosition = _treasureChest->GetTransform()->GetLocalPosition();
		Vec3 currentRotation = _treasureChest->GetTransform()->GetLocalRotation();

		if (currentPosition.y < 200.f)
		{
			_treasureChest->GetTransform()->SetLocalPosition(Vec3(0.0f, 150.f, 0.0f));
			_getChest = false;
		}
		else
		{
			_treasureChest->GetTransform()->SetLocalPosition(Vec3(0.0f, currentPosition.y - 11.0f, 0.0f));
			_treasureChest->GetTransform()->SetLocalRotation(currentRotation + Vec3(0.0f, 0.015f, 0.0f));
		}
	}
}

void LuckyScript::KeyboardInput()
{
	if (INPUT->GetButton(KEY_TYPE::U))
	{
		if (!_getChest) _getChest = true;

		auto particle_system = _treasureChest->GetParticleSystem();
		particle_system->ParticleStart();
	}
}

void LuckyScript::MouseInput()
{
}