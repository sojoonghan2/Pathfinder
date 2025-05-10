#include "pch.h"
#include "NetworkMonsterHpScript.h"

#include "MessageManager.h"
#include "Transform.h"

NetworkMonsterHpScript::NetworkMonsterHpScript()
{
}

NetworkMonsterHpScript::~NetworkMonsterHpScript()
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

	// ���⿡ ü�� ��ġ�� ���� ü�� ǥ���ϴ� �κ��� �־��� [�ΰ��]
	// ���� _maxHp�� �ִ� ü��, _currentHp�� ���� ü�� �������

}


void NetworkMonsterHpScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterStaticObject(ID_OBJECT_MONSTER_HP, _id);
}
