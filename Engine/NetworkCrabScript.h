#pragma once
#include "MonoBehaviour.h"

class NetworkCrabScript : public MonoBehaviour
{
public:
	NetworkCrabScript();
	virtual void LateUpdate() override;

	void CheckBulletHits();

	void SetPosition(float x, float z);
	void SetDir(float x, float z);

	void Animation();


private:

	int			_id{ -1 };
	float		_lastX{ -1.f };
	float		_lastY{ -1.f };
	bool		_isMove{ false };
	uint32		_currentAnimIndex{ 0 };

};
