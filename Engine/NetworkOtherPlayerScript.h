#pragma once
#include "MonoBehaviour.h"

class NetworkOtherPlayerScript : public MonoBehaviour
{
public:
	NetworkOtherPlayerScript();
	virtual ~NetworkOtherPlayerScript();

	virtual void LateUpdate() override;
	virtual void Awake() override;

	void SetPosition(float x, float z);
	void SetDir(float x, float z);

	void Animation();

private:
	float		_lastX{ -1.f };
	float		_lastY{ -1.f };
	bool		_isMove{ false };
	bool		_isShoot{ false };
	uint32		_currentAnimIndex{ 0 };

};
