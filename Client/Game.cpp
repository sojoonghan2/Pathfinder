#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	// ���� �ӽ÷� �������� ��Ű���� �ε�
	GET_SINGLE(SceneManager)->LoadScene(L"LuckyScene");
}

void Game::Update()
{
	GEngine->Update();
}
