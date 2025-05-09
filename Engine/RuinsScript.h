#pragma once
#include "MasterScript.h"
#include "GameModule.h"

class RuinsScript : public MasterScript
{
public:
	RuinsScript();
	virtual ~RuinsScript();

	virtual void LateUpdate() override;
	virtual void Awake() override;
	virtual void Start() override;

	void Occupation();
	void BlinkUI();
	void CreatePortal();
	void IsPortalZone();
	void ModuleSelect();
	void WaitOtherModule();

private:
	shared_ptr<GameObject>  _water;
	shared_ptr<GameObject>  _player;
	shared_ptr<GameObject> _occupationUI;

	shared_ptr<GameObject> _waitUI;
	shared_ptr<GameObject> _portalParticle;
	shared_ptr<GameObject> _portalFrameParticle;
	
	shared_ptr<GameModule> _module[3];

	float _blinkTime = 0.75f;
	float _elapsedTime = 0.0f;
	bool _isVisible = true;

	bool _initialized = false;
};
