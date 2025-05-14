#pragma once

#include "GameObject.h"
#include "Texture.h"
#include "Resources.h"

class GameModule : public GameObject
{
public:
	GameModule();
	void SetModuleAbility();

	void IncreaseATKUp();
	void IncreaseDEFUp();
	void IncreaseHPUp();
	void IncreaseCRTDamageUp();
	void IncreaseCRTRateUp();

	void IncreaseBattleFanatic();
	void IncreaseAdjust();
	void IncreaseOverHeating();
	void IncreaseGamester();
	void IncreaseEmergencyAssistance();
	void IncreaseArmor();
	void IncreaseEnhancedSiege();

	shared_ptr<Texture> GetTexture() { return texture; }
	int GetModuleType() { return moduleType; }

private:
	shared_ptr<Texture> texture;
	int moduleType;
};