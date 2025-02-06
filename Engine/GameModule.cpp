#include "pch.h"
#include "GameModule.h"

GameModule::GameModule()
{
	// ��� ����
	SetModuleAbility();
}

void GameModule::SetModuleAbility()
{
	static std::random_device rd;
	static std::default_random_engine dre(rd());
	static std::uniform_int_distribution<> uid(0, 11);

	// �Լ� ������ �迭
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

	// ������ �Լ� ���� �� ȣ��
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::ATKUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "����\n";
}

void GameModule::DEFUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "����\n";
}

void GameModule::HPUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "ü��\n";
}

void GameModule::CRTDamageUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "ġ����\n";
}

void GameModule::CRTRateUp()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "ġȮ��\n";
}

void GameModule::BattleFanatic()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "������\n";
}

void GameModule::Adjust()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "����\n";
}

void GameModule::OverHeating()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "����\n";
}

void GameModule::Gamester()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "���ڲ�\n";
}

void GameModule::EmergencyAssistance()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "���� ����\n";
}

void GameModule::Armor()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "ö��\n";
}

void GameModule::IncreasedFirepower()
{
	texture = GET_SINGLE(Resources)->Load<Texture>(L"TestModule",
		L"..\\Resources\\Texture\\Module\\TestModule.png");
	std::cout << "ȭ�� ��ȭ\n";
}
