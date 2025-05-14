#pragma once
#include "MonoBehaviour.h"


class NetworkBulletScript : public MonoBehaviour
{
public:

	NetworkBulletScript();
	virtual ~NetworkBulletScript();

	virtual void LateUpdate() override;
	virtual void Awake() override;


	void SetPosition(float x, float z);
	void SetDir(float x, float z);

private:

	bool _isFired = false;

};

