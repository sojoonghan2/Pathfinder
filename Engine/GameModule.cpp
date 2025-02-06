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
		&GameModule::EnhancedSiege
	};

	// 랜덤한 함수 선택 및 호출
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::ATKUp()
{
	// 공격력 증폭
	// 공격력이 3%/5%/9% 증가한다.
	texture = GET_SINGLE(Resources)->Load<Texture>(L"ATKUp",
		L"..\\Resources\\Texture\\Module\\ATKUp.png");
}

void GameModule::DEFUp()
{
	// 방어력 증폭
	// 방어력이 2%/4%/7% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"DEFUp",
		L"..\\Resources\\Texture\\Module\\DEFUp.png");
}

void GameModule::HPUp()
{
	// 체력 증폭
	// 체력이 10%/15%/25% 증가한
	texture = GET_SINGLE(Resources)->Load<Texture>(L"HPUp",
		L"..\\Resources\\Texture\\Module\\HPUp.png");
}

void GameModule::CRTDamageUp()
{
	// 치명타 데미지 증폭
	// 치명타 데미지가 10%/20%/40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTDamageUp",
		L"..\\Resources\\Texture\\Module\\CRTDamageUp.png");
}

void GameModule::CRTRateUp()
{
	// 치명타 확률 증폭
	// 치명타 확률이 10%/15%/30% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTRateUp",
		L"..\\Resources\\Texture\\Module\\CRTRateUp.png");
}

void GameModule::BattleFanatic()
{
	// 전투광
	// 관문 시작 시 이동속도가 15초간 20% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"BattleFanatic",
		L"..\\Resources\\Texture\\Module\\BattleFanatic.png");
}

void GameModule::Adjust()
{
	// 적응
	// 20초마다 공격력이 2% 올라가는 버프를 6회(총12%) 획득한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Adjust",
		L"..\\Resources\\Texture\\Module\\Adjust.png");
}

void GameModule::OverHeating()
{
	// 과열
	// 체력이 10% 감소하고, 공격력이 40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"OverHeating",
		L"..\\Resources\\Texture\\Module\\OverHeating.png");
}

void GameModule::Gamester()
{
	// 도박꾼
	// 이 모듈을 2회 획득하면 체력이 100% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Gamester",
		L"..\\Resources\\Texture\\Module\\Gamester.png");
}

void GameModule::EmergencyAssistance()
{
	// 응급 지원
	// 힐러 직업군의 회복량이 100% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EmergencyAssistance",
		L"..\\Resources\\Texture\\Module\\EmergencyAssistance.png");
}

void GameModule::Armor()
{
	// 철갑
	// 탱커 직업군의 방어력이 40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Armor",
		L"..\\Resources\\Texture\\Module\\Armor.png");
}

void GameModule::EnhancedSiege()
{
	// 화력 강화
	// 딜러 직업군의 공격력이 60% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EnhancedSiege",
		L"..\\Resources\\Texture\\Module\\EnhancedSiege.png");
}
