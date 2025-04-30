#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

#include "LoadingScene.h"
#include "TitleScene.h"
#include "ParticleScene.h"
#include "RuinsScene.h"
#include "FactoryScene.h"
#include "ExplorationScene.h"
#include "CrashScene.h"
#include "LuckyScene.h"
#include "BossScene.h"
#include "TestScene.h"

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

#ifdef ALLLOAD
	shared_ptr<TitleScene> titleScene = make_shared<TitleScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"TitleScene", titleScene);
	titleScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"TitleScene");
#endif

#ifdef TITLELOAD
	shared_ptr<TitleScene> titleScene = make_shared<TitleScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"TitleScene", titleScene);
	titleScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"TitleScene");
#endif

#ifdef RUINSLOAD
	shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", ruinsScene);
	ruinsScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
#endif

#ifdef FACTORYLOAD
	shared_ptr<FactoryScene> factoryScene = make_shared<FactoryScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"FactoryScene", factoryScene);
	factoryScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
#endif

#ifdef EXPLORATIONLOAD
	shared_ptr<ExplorationScene> explorationScene = make_shared<ExplorationScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"ExplorationScene", explorationScene);
	explorationScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"ExplorationScene");
#endif

#ifdef CRASHLOAD
	shared_ptr<CrashScene> crashScene = make_shared<CrashScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"CrashScene", crashScene);
	crashScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"CrashScene");
#endif

#ifdef LUCKYLOAD
	shared_ptr<LuckyScene> luckyScene = make_shared<LuckyScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"LuckyScene", luckyScene);
	luckyScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"LuckyScene");
#endif

#ifdef BOSSLOAD
	shared_ptr<BossScene> bossScene = make_shared<BossScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"BossScene", bossScene);
	bossScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"BossScene");
#endif

#ifdef PARTICLELOAD
	shared_ptr<ParticleScene> particleScene = make_shared<ParticleScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"ParticleScene", particleScene);
	particleScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"ParticleScene");
#endif

#ifdef TESTLOAD
	shared_ptr<TestScene> testScene = make_shared<TestScene>();
	GET_SINGLE(SceneManager)->RegisterScene(L"TestScene", testScene);
	testScene->Init();
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
#endif
}

void Game::Update()
{
	GEngine->Update();
}