#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Timer.h"

TitleScript::TitleScript() {}

TitleScript::~TitleScript() {}

void TitleScript::LateUpdate() {
	// 게임 윈도우가 포커스 되었을 때만 입력 처리
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::N))
		{
			_isMatch = true;
		}
		if (INPUT->GetButton(KEY_TYPE::M))
		{
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
	}

	if (_isMatch) MatchMaking();
}

void TitleScript::MatchMaking()
{
	auto icon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	if (icon)
	{
		icon->SetRenderOn();

		Vec3 rotation = icon->GetTransform()->GetLocalRotation();
		rotation.z += DELTA_TIME * 3.f;
		icon->GetTransform()->SetLocalRotation(rotation);
	}
}
