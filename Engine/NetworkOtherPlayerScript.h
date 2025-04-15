#pragma once
#include "MonoBehaviour.h"

class NetworkOtherPlayerScript : public MonoBehaviour
{
public:
	NetworkOtherPlayerScript();
	virtual ~NetworkOtherPlayerScript();

	virtual void LateUpdate() override;

	void SetPosition(float x, float z);
	void SetDir(float x, float z);

private:
	float		_speed = PLAYER_SPEED_MPS * METER_TO_CLIENT;
	int			id = -1;
};
