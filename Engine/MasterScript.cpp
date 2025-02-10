#include "pch.h"
#include "MasterScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"

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
	// TODO:
	// 플레이어 클래스를 만들게 되면
	// 플레이어를 포인터로 가지고 있고(생성자에서 Player 할당)
	// 모듈을 선택하면 해당 플레이어의 스텟 증가 함수를 만들어
	// 모듈을 적용시키면 된다.

	auto type = GET_SINGLE(SceneManager)->GetActiveScene()->GetSelectedModuleType();
	std::cout << "Module Type: " << type + 1 << "\n";
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