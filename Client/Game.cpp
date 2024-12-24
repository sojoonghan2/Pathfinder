#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	// 현재 임시로 개발중인 럭키씬을 로드
	GET_SINGLE(SceneManager)->LoadScene(L"LuckyScene");
}

void Game::Update()
{
	GEngine->Update();
}
