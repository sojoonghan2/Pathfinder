#pragma once
#include "MonoBehaviour.h"
#include "PlayerScript.h"

class RazerParticleScript : public MonoBehaviour
{
public:
	RazerParticleScript(shared_ptr<PlayerScript> playerScript);
	virtual ~RazerParticleScript();

	virtual void LateUpdate() override;

private:
	float                       _delayTimer;
	float                       _cooldownTimer;
	float                       _cooldownDuration = 10.f;

	shared_ptr<PlayerScript>    _playerScript;
};
