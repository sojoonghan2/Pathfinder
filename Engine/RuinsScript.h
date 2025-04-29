#pragma once
#include "MasterScript.h"

class RuinsScript : public MasterScript
{
public:
	RuinsScript();
	virtual ~RuinsScript();

	virtual void LateUpdate() override;
	virtual void Start() override;
	virtual void Awake() override;

	void Occupation();
	void BlinkUI();
	void CreatePortal();

private:
	shared_ptr<GameObject>  _water;
	shared_ptr<GameObject>  _player;
	shared_ptr<GameObject> _occupationUI;

	shared_ptr<GameObject> _waitUI;
	shared_ptr<GameObject> _portalParticle;
	shared_ptr<GameObject> _portalFrameParticle;


	float _blinkTime = 0.75f;
	float _elapsedTime = 0.0f;
	bool _isVisible = true;

	bool _initialized = false;
};
