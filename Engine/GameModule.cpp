#include "pch.h"
#include "GameModule.h"

GameModule::GameModule()
{
	// 모듈 선택
	SetModuleAbility();
}

void GameModule::SetModuleAbility()
{
	static std::random_device rd;
	static std::default_random_engine dre(rd());
	static std::uniform_int_distribution<> uid(0, 11);

	// 함수 포인터 배열
	void (GameModule:: * abilities[])() = {
		&GameModule::ATKUp,
		&GameModule::DEFUp,
		&GameModule::HPUp,
		&GameModule::CRTDamageUp,
		&GameModule::CRTRateUp,
		&GameModule::BattleFanatic,
		&GameModule::Adjust,
		&GameModule::OverHeating,
		&GameModule::Gamester,
		&GameModule::EmergencyAssistance,
		&GameModule::Armor,
		&GameModule::IncreasedFirepower
	};

	// 랜덤한 함수 선택 및 호출
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::ATKUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "공증\n";
}

void GameModule::DEFUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "방증\n";
}

void GameModule::HPUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "체증\n";
}

void GameModule::CRTDamageUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "치뎀증\n";
}

void GameModule::CRTRateUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "치확증\n";
}

void GameModule::BattleFanatic()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "전투광\n";
}

void GameModule::Adjust()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "적응\n";
}

void GameModule::OverHeating()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "과열\n";
}

void GameModule::Gamester()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "도박꾼\n";
}

void GameModule::EmergencyAssistance()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "응급 지원\n";
}

void GameModule::Armor()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "철갑\n";
}

void GameModule::IncreasedFirepower()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "화력 강화\n";
}
