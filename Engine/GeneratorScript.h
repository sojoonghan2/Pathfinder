#pragma once
#include "MonoBehaviour.h"

class GeneratorScript : public MonoBehaviour
{
public:
	GeneratorScript();

	virtual void LateUpdate() override;

private:
	bool _isPlaying = false;
	bool _init = false;

	float _deadTime;
};

