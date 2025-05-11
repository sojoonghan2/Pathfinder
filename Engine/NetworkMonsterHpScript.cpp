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

	// ���⿡ ü�� ��ġ�� ���� ü�� ǥ���ϴ� �κ��� �־��� [�ΰ��]
	// ���� _maxHp�� �ִ� ü��, _currentHp�� ���� ü�� �������

	float fullWidth = 480.f;
	float hpRatio = _currentHp / _maxHp;

	Vec3 hpScale = GetGameObject()->GetTransform()->GetLocalScale();
	Vec3 hpPos = GetGameObject()->GetTransform()->GetLocalPosition();

	hpScale.x = fullWidth * hpRatio;
	hpPos.x = (fullWidth * 0.5f) * hpRatio - 240.f; // ������ �𸣰ڴµ� �ڲ� ü�¹ٰ� ����� ���� ���ݰ���ŭ ����
	
	GetGameObject()->GetTransform()->SetLocalScale(hpScale);
	GetGameObject()->GetTransform()->SetLocalPosition(hpPos);
	
	// ������ scale�� 0�̿� �ϴµ� �ǰ� 1�븸ŭ ����
	// �׷��ٰ� _currentHp == 0�϶� GetGameObject()->SetRender(false) �ع����� �ƿ� ü�¹� ����� �ȵ�
	// ü�� ��� �ڵ带 �� �� �ҷ��� �� �ǰ� 1�� ����
	// ������ Update�� �Ⱥҷ��� �׷���
}


void NetworkMonsterHpScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterStaticObject(ID_OBJECT_MONSTER_HP, _id);
}
