#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

#ifdef ALLLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
#endif

#ifdef TITLELOAD
	GET_SINGLE(SceneManager)->LoadScene(L"TitleScene");
#endif

#ifdef RUINSLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
#endif

#ifdef FACTORYLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
#endif

#ifdef EXPLORATIONLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"ExplorationScene");
#endif

#ifdef CRASHLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"CrashScene");
#endif

#ifdef LUCKYLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"LuckyScene");
#endif

#ifdef BOSSLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"BossScene");
#endif

#ifdef PARTICLELOAD
	GET_SINGLE(SceneManager)->LoadScene(L"ParticleScene");
#endif

#ifdef TESTLOAD
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
#endif

}

void Game::Update()
{
	GEngine->Update();
}
