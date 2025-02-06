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
		&GameModule::EnhancedSiege
	};

	// ������ �Լ� ���� �� ȣ��
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::ATKUp()
{
	// ���ݷ� ����
	// ���ݷ��� 3%/5%/9% �����Ѵ�.
	texture = GET_SINGLE(Resources)->Load<Texture>(L"ATKUp",
		L"..\\Resources\\Texture\\Module\\ATKUp.png");
}

void GameModule::DEFUp()
{
	// ���� ����
	// ������ 2%/4%/7% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"DEFUp",
		L"..\\Resources\\Texture\\Module\\DEFUp.png");
}

void GameModule::HPUp()
{
	// ü�� ����
	// ü���� 10%/15%/25% ������
	texture = GET_SINGLE(Resources)->Load<Texture>(L"HPUp",
		L"..\\Resources\\Texture\\Module\\HPUp.png");
}

void GameModule::CRTDamageUp()
{
	// ġ��Ÿ ������ ����
	// ġ��Ÿ �������� 10%/20%/40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTDamageUp",
		L"..\\Resources\\Texture\\Module\\CRTDamageUp.png");
}

void GameModule::CRTRateUp()
{
	// ġ��Ÿ Ȯ�� ����
	// ġ��Ÿ Ȯ���� 10%/15%/30% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTRateUp",
		L"..\\Resources\\Texture\\Module\\CRTRateUp.png");
}

void GameModule::BattleFanatic()
{
	// ������
	// ���� ���� �� �̵��ӵ��� 15�ʰ� 20% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"BattleFanatic",
		L"..\\Resources\\Texture\\Module\\BattleFanatic.png");
}

void GameModule::Adjust()
{
	// ����
	// 20�ʸ��� ���ݷ��� 2% �ö󰡴� ������ 6ȸ(��12%) ȹ���Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Adjust",
		L"..\\Resources\\Texture\\Module\\Adjust.png");
}

void GameModule::OverHeating()
{
	// ����
	// ü���� 10% �����ϰ�, ���ݷ��� 40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"OverHeating",
		L"..\\Resources\\Texture\\Module\\OverHeating.png");
}

void GameModule::Gamester()
{
	// ���ڲ�
	// �� ����� 2ȸ ȹ���ϸ� ü���� 100% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Gamester",
		L"..\\Resources\\Texture\\Module\\Gamester.png");
}

void GameModule::EmergencyAssistance()
{
	// ���� ����
	// ���� �������� ȸ������ 100% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EmergencyAssistance",
		L"..\\Resources\\Texture\\Module\\EmergencyAssistance.png");
}

void GameModule::Armor()
{
	// ö��
	// ��Ŀ �������� ������ 40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Armor",
		L"..\\Resources\\Texture\\Module\\Armor.png");
}

void GameModule::EnhancedSiege()
{
	// ȭ�� ��ȭ
	// ���� �������� ���ݷ��� 60% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EnhancedSiege",
		L"..\\Resources\\Texture\\Module\\EnhancedSiege.png");
}
