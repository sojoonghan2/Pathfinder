#include "pch.h"
#include "MasterScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

MasterScript::MasterScript()
{
}

MasterScript::~MasterScript()
{
}

void MasterScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void MasterScript::KeyboardInput()
{
    
}

void MasterScript::MouseInput()
{
	
}

void MasterScript::ApplyModuleAbillities()
{
	// activeScene의 selectedModuleType을 가져와서
	// 아래의 switch문과 비교 후
	// 플레이어를 강화
	int type{};
	switch (type)
	{
	case ATKUp:
		break;
	case DEFUp:
		break;
	case HPUp:
		break;
	case CRTDamageUp:
		break;
	case CRTRateUp:
		break;
	case BattleFanatic:
		break;
	case Adjust:
		break;
	case OverHeating:
		break;
	case Gamester:
		break;
	case EmergencyAssistance:
		break;
	case Armor:
		break;
	case EnhancedSiege:
		break;
	}
}
