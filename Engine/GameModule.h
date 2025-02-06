#pragma once

#include "GameObject.h"
#include "Texture.h"
#include "Resources.h"

class GameModule : public GameObject
{
public:
	GameModule();
	void SetModuleAbility();

	void ATKUp();
	void DEFUp();
	void HPUp();
	void CRTDamageUp();
	void CRTRateUp();

	void BattleFanatic();
	void Adjust();
	void OverHeating();
	void Gamester();
	void EmergencyAssistance();
	void Armor();
	void EnhancedSiege();

	shared_ptr<Texture> GetTexture() { return texture; }

private:
	// TODO:
	// �÷��̾� Ŭ������ ����� �Ǹ�
	// �÷��̾ �����ͷ� ������ �ְ�(�����ڿ��� Player �Ҵ�)
	// ����� �����ϸ� �ش� �÷��̾��� ���� ���� �Լ��� �����
	// ����� �����Ű�� �ȴ�.

	shared_ptr<Texture> texture;
};