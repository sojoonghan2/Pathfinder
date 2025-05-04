#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Timer.h"
#include "SocketIO.h"
#include "LoadingScene.h"

TitleScript::TitleScript() {}
TitleScript::~TitleScript() {}

void TitleScript::Start()
{
	_matchingIcon = GET_SINGLE(SceneManager)->FindObjectByName(L"MatchingIcon");
	_loadingIcon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	_matchmakingIng = GET_SINGLE(SceneManager)->FindObjectByName(L"Matchmaking_ing");

	if (_matchingIcon)
		_currentScale = _matchingIcon->GetTransform()->GetLocalScale();
}

void TitleScript::LateUpdate()
{
	const POINT& pos = INPUT->GetMousePos();

	bool isMouseOnButton = (pos.x >= 850 && pos.x <= 1200 &&
		pos.y >= 530 && pos.y <= 700);

	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(MOUSE_TYPE::LBUTTON) && isMouseOnButton)
		{
#ifdef NETWORK_ENABLE
			if (false == _isMatch)
			{
				GET_SINGLE(SocketIO)->DoSend<packet::CSMatchmaking>();
				INPUT->LockCursor(true);
			}
#endif
			_isMatch = true;
		}
		if (INPUT->GetButton(KEY_TYPE::M))
		{
			INPUT->LockCursor(true);
			shared_ptr<LoadingScene> loadingScene = make_shared<LoadingScene>();
			GET_SINGLE(SceneManager)->RegisterScene(L"LoadingScene", loadingScene);
			loadingScene->Init(RoomType::Ruin);
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
	}

	if (isMouseOnButton)
		HoveredAnimation();
	else
		ResetScale();

	if (_matchingIcon)
	{
		const float speed = 10.f;
		_currentScale = Vec3::Lerp(_currentScale, _targetScale, DELTA_TIME * speed);
		_matchingIcon->GetTransform()->SetLocalScale(_currentScale);
	}

	if (_isMatch)
		MatchMaking();
}

void TitleScript::HoveredAnimation()
{
	if (_matchingIcon)
		_targetScale = Vec3(500.f, 500.f, 1.f);
}

void TitleScript::ResetScale()
{
	if (_matchingIcon)
		_targetScale = Vec3(450.f, 450.f, 1.f);
}

void TitleScript::MatchMaking()
{
	if (_loadingIcon)
	{
		_loadingIcon->SetRender(true);
	}

	if (_matchmakingIng)
	{
		_matchmakingIng->SetRender(true);
	}

	if (_loadingIcon)
	{
		Vec3 rotation = _loadingIcon->GetTransform()->GetLocalRotation();
		rotation.z += DELTA_TIME * 3.f;
		_loadingIcon->GetTransform()->SetLocalRotation(rotation);
	}
}