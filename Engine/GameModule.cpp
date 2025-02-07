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

	// ������ �Լ� ���� �� ȣ��
	int randomIndex = uid(dre);
	(this->*abilities[randomIndex])();
}

void GameModule::IncreaseATKUp()
{
	// ���ݷ� ����
	// ���ݷ��� 3%/5%/9% �����Ѵ�.
	texture = GET_SINGLE(Resources)->Load<Texture>(L"ATKUp",
		L"..\\Resources\\Texture\\Module\\ATKUp.png");
	moduleType = ATKUp;
}

void GameModule::IncreaseDEFUp()
{
	// ���� ����
	// ������ 2%/4%/7% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"DEFUp",
		L"..\\Resources\\Texture\\Module\\DEFUp.png");
	moduleType = DEFUp;
}

void GameModule::IncreaseHPUp()
{
	// ü�� ����
	// ü���� 10%/15%/25% ������
	texture = GET_SINGLE(Resources)->Load<Texture>(L"HPUp",
		L"..\\Resources\\Texture\\Module\\HPUp.png");
	moduleType = HPUp;
}

void GameModule::IncreaseCRTDamageUp()
{
	// ġ��Ÿ ������ ����
	// ġ��Ÿ �������� 10%/20%/40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTDamageUp",
		L"..\\Resources\\Texture\\Module\\CRTDamageUp.png");
	moduleType = CRTDamageUp;
}

void GameModule::IncreaseCRTRateUp()
{
	// ġ��Ÿ Ȯ�� ����
	// ġ��Ÿ Ȯ���� 10%/15%/30% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"CRTRateUp",
		L"..\\Resources\\Texture\\Module\\CRTRateUp.png");
	moduleType = CRTRateUp;
}

void GameModule::IncreaseBattleFanatic()
{
	// ������
	// ���� ���� �� �̵��ӵ��� 15�ʰ� 20% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"BattleFanatic",
		L"..\\Resources\\Texture\\Module\\BattleFanatic.png");
	moduleType = BattleFanatic;
}

void GameModule::IncreaseAdjust()
{
	// ����
	// 20�ʸ��� ���ݷ��� 2% �ö󰡴� ������ 6ȸ(��12%) ȹ���Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Adjust",
		L"..\\Resources\\Texture\\Module\\Adjust.png");
	moduleType = Adjust;
}

void GameModule::IncreaseOverHeating()
{
	// ����
	// ü���� 10% �����ϰ�, ���ݷ��� 40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"OverHeating",
		L"..\\Resources\\Texture\\Module\\OverHeating.png");
	moduleType = OverHeating;
}

void GameModule::IncreaseGamester()
{
	// ���ڲ�
	// �� ����� 2ȸ ȹ���ϸ� ü���� 100% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Gamester",
		L"..\\Resources\\Texture\\Module\\Gamester.png");
	moduleType = Gamester;
}

void GameModule::IncreaseEmergencyAssistance()
{
	// ���� ����
	// ���� �������� ȸ������ 100% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EmergencyAssistance",
		L"..\\Resources\\Texture\\Module\\EmergencyAssistance.png");
	moduleType = EmergencyAssistance;
}

void GameModule::IncreaseArmor()
{
	// ö��
	// ��Ŀ �������� ������ 40% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"Armor",
		L"..\\Resources\\Texture\\Module\\Armor.png");
	moduleType = Armor;
}

void GameModule::IncreaseEnhancedSiege()
{
	// ȭ�� ��ȭ
	// ���� �������� ���ݷ��� 60% �����Ѵ�
	texture = GET_SINGLE(Resources)->Load<Texture>(L"EnhancedSiege",
		L"..\\Resources\\Texture\\Module\\EnhancedSiege.png");
	moduleType = EnhancedSiege;
}
