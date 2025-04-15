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


private:

	int _id{ -1 };
};
