#pragma once
#include "Component.h"

// ����� ���� ��ũ��Ʈ ����� �����ϱ� ���� ��� ������Ʈ Ŭ����
// MonoBehaviour�� ��ӹ޾� Ŀ���� ������Ʈ�� ����
// ���ǿ��� ��� ���� ȿ������ ����
class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

private:
	virtual void FinalUpdate() sealed {}
};
