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

	void Animation();

private:
	int			_id{ -1 };
	float		_lastX{ -1.f };
	float		_lastY{ -1.f };
	bool		_isMove{ true };
	uint32		_currentAnimIndex{ 0 };

};
