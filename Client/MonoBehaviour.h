#pragma once
#include "Component.h"

// 사용자 정의 스크립트 기능을 구현하기 위한 기반 컴포넌트 클래스
// MonoBehaviour를 상속받아 커스텀 컴포넌트를 정의
// 강의에서 배운 가장 효율적인 구조
class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

private:
	virtual void FinalUpdate() sealed {}
};
