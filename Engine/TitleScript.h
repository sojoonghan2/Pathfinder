#pragma once
#include "MasterScript.h"

class TitleScript : public MasterScript
{
public:
	TitleScript();
	virtual ~TitleScript();

	virtual void Start() override;
	virtual void LateUpdate() override;

	void HoveredAnimation();
	void ResetScale();
	void MatchMaking();

private:
	bool						_isMatch = false;
	bool						_isHovered = false;

	shared_ptr<GameObject>		_matchingIcon;
	shared_ptr<GameObject>		_loadingIcon;

	Vec3 _currentScale = Vec3(450.f, 450.f, 1.f);
	Vec3 _targetScale = Vec3(450.f, 450.f, 1.f);
};
