#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Timer.h"
#include "SocketIO.h"
#include "Input.h"

TitleScript::TitleScript() {}

TitleScript::~TitleScript() {}

void TitleScript::Start()
{
	_matchingIcon = GET_SINGLE(SceneManager)->FindObjectByName(L"MatchingIcon");
	_loadingIcon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
}

void TitleScript::LateUpdate() {
	const POINT& pos = INPUT->GetMousePos();

	bool isMouseOnButton = (pos.x >= 850 && pos.x <= 1200 &&
		pos.y >= 530 && pos.y <= 700);

	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::LBUTTON) && isMouseOnButton)
		{
#ifdef NETWORK_ENABLE
			if (false == _isMatch)
				GET_SINGLE(SocketIO)->DoSend<packet::CSMatchmaking>();
#endif
			_isMatch = true;
		}
		if (INPUT->GetButton(KEY_TYPE::M))
		{
			cout << "m\n";
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
	}

	if (isMouseOnButton)
	{
		HoveredAnimation();
	}
	else
	{
		ResetScale();
	}

	if (_matchingIcon)
	{
		const float speed = 10.f;
		_currentScale = Vec3::Lerp(_currentScale, _targetScale, DELTA_TIME * speed);
		_matchingIcon->GetTransform()->SetLocalScale(_currentScale);
	}

	if (_isMatch) MatchMaking();
}

void TitleScript::HoveredAnimation()
{
	if (_matchingIcon)
	{
		_targetScale = Vec3(500.f, 500.f, 1.f);
	}
}

void TitleScript::ResetScale()
{
	if (_matchingIcon)
	{
		_targetScale = Vec3(450.f, 450.f, 1.f);
	}
}

void TitleScript::MatchMaking()
{
	if (_loadingIcon)
	{
		_loadingIcon->SetRenderOn();

		auto matchmakingIng = GET_SINGLE(SceneManager)->FindObjectByName(L"Matchmaking_ing");
		matchmakingIng->SetRenderOn();

		Vec3 rotation = _loadingIcon->GetTransform()->GetLocalRotation();
		rotation.z += DELTA_TIME * 3.f;
		_loadingIcon->GetTransform()->SetLocalRotation(rotation);
	}
}