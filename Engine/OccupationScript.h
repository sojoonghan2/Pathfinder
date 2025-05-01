#pragma once
#include "MonoBehaviour.h"

class OccupationScript : public MonoBehaviour
{
public:
	virtual void LateUpdate() override;

private:
	float m_accumulatedTime = 0.0f;
};