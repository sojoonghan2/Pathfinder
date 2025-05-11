#include "pch.h"
#include "NetworkMonsterHpScript.h"

#include "MessageManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "SceneManager.h"

NetworkMonsterHpScript::NetworkMonsterHpScript()
{
}

NetworkMonsterHpScript::~NetworkMonsterHpScript()
{
}

void NetworkMonsterHpScript::Start()
{
}

void NetworkMonsterHpScript::LateUpdate()
{

	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(_id);
	while (not queue.empty()) {
		auto& message{ queue.front() };
		switch (message->type) {
		case MsgType::SET_OBJECT_HP:
		{
			auto message_set_hp{ std::static_pointer_cast<MsgSetObjectHp>(message) };
			_maxHp = message_set_hp->maxHp;
			_currentHp = message_set_hp->hp;
			std::cout << "ATTACKER Crab HP : " << _currentHp << std::endl;
		}
		break;
		default:
			break;
		}
		queue.pop();
	}

	if (_maxHp <= -1.f && _showHp) {
		_showHp = false;
	}
	if (_maxHp > -1.f && not _showHp) {
		_showHp = true;
	}

	// 여기에 체력 수치에 따라 체력 표시하는 부분을 넣어줘 [민경원]
	// 현재 _maxHp에 최대 체력, _currentHp에 현재 체력 들어있음

	float fullWidth = 480.f;
	float hpRatio = _currentHp / _maxHp;

	Vec3 hpScale = GetGameObject()->GetTransform()->GetLocalScale();
	Vec3 hpPos = GetGameObject()->GetTransform()->GetLocalPosition();

	hpScale.x = fullWidth * hpRatio;
	hpPos.x = (fullWidth * 0.5f) * hpRatio - 240.f; // 왜인지 모르겠는데 자꾸 체력바가 가운데로 가서 절반값만큼 빼줌
	
	GetGameObject()->GetTransform()->SetLocalScale(hpScale);
	GetGameObject()->GetTransform()->SetLocalPosition(hpPos);
	
	// 죽으면 scale이 0이여 하는데 피가 1대만큼 남음
	// 그렇다고 _currentHp == 0일때 GetGameObject()->SetRender(false) 해버리면 아예 체력바 출력이 안됨
	// 체력 깎는 코드를 두 번 불러도 꼭 피가 1대 남음
	// 죽으면 Update가 안불려서 그런가
}


void NetworkMonsterHpScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterStaticObject(ID_OBJECT_MONSTER_HP, _id);
}
