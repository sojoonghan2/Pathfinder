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
	// �÷��̾� Ŭ������ ����� �Ǹ�
	// �÷��̾ �����ͷ� ������ �ְ�(�����ڿ��� Player �Ҵ�)
	// ����� �����ϸ� �ش� �÷��̾��� ���� ���� �Լ��� �����
	// ����� �����Ű�� �ȴ�.

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