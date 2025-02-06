#include "pch.h"
#include "GameModule.h"

GameModule::GameModule()
{
	SetModuleAbility();
}

void GameModule::SetModuleAbility()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
}

void GameModule::ATKUp()
{
}

void GameModule::DEFUp()
{
}

void GameModule::HPUp()
{
}

void GameModule::CRTDamageUp()
{
}

void GameModule::CRTRateUp()
{
}

void GameModule::BattleFanatic()
{
}

void GameModule::Adjust()
{
}

void GameModule::OverHeating()
{
}

void GameModule::Gamester()
{
}

void GameModule::EmergencyAssistance()
{
}

void GameModule::Armor()
{
}

void GameModule::IncreasedFirepower()
{
}
