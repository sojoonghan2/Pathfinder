#pragma once
#include "MonoBehaviour.h"


class NetworkMonsterHpScript : public MonoBehaviour
{
public:

	NetworkMonsterHpScript();
	virtual ~NetworkMonsterHpScript();

	virtual void LateUpdate() override;
	virtual void Awake() override;

private:
	float _maxHp{ -1.f };
	float _currentHp{ 0.f };

	bool  _showHp{ false };

};

