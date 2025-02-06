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
	// 플레이어 클래스를 만들게 되면
	// 플레이어를 포인터로 가지고 있고(생성자에서 Player 할당)
	// 모듈을 선택하면 해당 플레이어의 스텟 증가 함수를 만들어
	// 모듈을 적용시키면 된다.

	shared_ptr<Texture> texture;
};