#pragma once
#include "MasterScript.h"

class RuinsScript : public MasterScript
{
public:
	RuinsScript();
	virtual ~RuinsScript();

	virtual void LateUpdate() override;

	void Occupation();
	void BlinkUI();

private:
	shared_ptr<GameObject>  _water;
	shared_ptr<GameObject>  _player;
	shared_ptr<GameObject> _occupationUI;

	float _blinkTime = 0.75f;
	float _elapsedTime = 0.0f;
	bool _isVisible = true;
};
