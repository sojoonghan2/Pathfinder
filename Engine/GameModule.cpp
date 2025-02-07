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
		&GameModule::IncreaseATKUp,
		&GameModule::IncreaseDEFUp,
		&GameModule::IncreaseHPUp,
		&GameModule::IncreaseCRTDamageUp,
		&GameModule::IncreaseCRTRateUp,
		&GameModule::IncreaseBattleFanatic,
		&GameModule::IncreaseAdjust,
		&GameModule::IncreaseOverHeating,
		&GameModule::IncreaseGamester,
		&GameModule::IncreaseEmergencyAssistance,
		&GameModule::IncreaseArmor,
		&GameModule::IncreaseEnhancedSiege
	};

	// 랜덤한 함수 선택 및 호출
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::IncreaseATKUp()
{
	// 공격력 증폭
	// 공격력이 3%/5%/9% 증가한다.
	texture = GET_SINGLE(Resources)->Load<Texture>(L"ATKUp",
		L"..\\Resources\\Texture\\Module\\ATKUp.png");
	moduleType = ATKUp;
}

void GameModule::IncreaseDEFUp()
{
	// 방어력 증폭
	// 방어력이 2%/4%/7% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"DEFUp",
		L"..\\Resources\\Texture\\Module\\DEFUp.png");
	moduleType = DEFUp;
}

void GameModule::IncreaseHPUp()
{
	// 체력 증폭
	// 체력이 10%/15%/25% 증가한
	texture = GET_SINGLE(Resources)->Load<Texture>(L"HPUp",
		L"..\\Resources\\Texture\\Module\\HPUp.png");
	moduleType = HPUp;
}

void GameModule::IncreaseCRTDamageUp()
{
	// 치명타 데미지 증폭
	// 치명타 데미지가 10%/20%/40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTDamageUp",
		L"..\\Resources\\Texture\\Module\\CRTDamageUp.png");
	moduleType = CRTDamageUp;
}

void GameModule::IncreaseCRTRateUp()
{
	// 치명타 확률 증폭
	// 치명타 확률이 10%/15%/30% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTRateUp",
		L"..\\Resources\\Texture\\Module\\CRTRateUp.png");
	moduleType = CRTRateUp;
}

void GameModule::IncreaseBattleFanatic()
{
	// 전투광
	// 관문 시작 시 이동속도가 15초간 20% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"BattleFanatic",
		L"..\\Resources\\Texture\\Module\\BattleFanatic.png");
	moduleType = BattleFanatic;
}

void GameModule::IncreaseAdjust()
{
	// 적응
	// 20초마다 공격력이 2% 올라가는 버프를 6회(총12%) 획득한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Adjust",
		L"..\\Resources\\Texture\\Module\\Adjust.png");
	moduleType = Adjust;
}

void GameModule::IncreaseOverHeating()
{
	// 과열
	// 체력이 10% 감소하고, 공격력이 40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"OverHeating",
		L"..\\Resources\\Texture\\Module\\OverHeating.png");
	moduleType = OverHeating;
}

void GameModule::IncreaseGamester()
{
	// 도박꾼
	// 이 모듈을 2회 획득하면 체력이 100% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Gamester",
		L"..\\Resources\\Texture\\Module\\Gamester.png");
	moduleType = Gamester;
}

void GameModule::IncreaseEmergencyAssistance()
{
	// 응급 지원
	// 힐러 직업군의 회복량이 100% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EmergencyAssistance",
		L"..\\Resources\\Texture\\Module\\EmergencyAssistance.png");
	moduleType = EmergencyAssistance;
}

void GameModule::IncreaseArmor()
{
	// 철갑
	// 탱커 직업군의 방어력이 40% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Armor",
		L"..\\Resources\\Texture\\Module\\Armor.png");
	moduleType = Armor;
}

void GameModule::IncreaseEnhancedSiege()
{
	// 화력 강화
	// 딜러 직업군의 공격력이 60% 증가한다
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EnhancedSiege",
		L"..\\Resources\\Texture\\Module\\EnhancedSiege.png");
	moduleType = EnhancedSiege;
}
