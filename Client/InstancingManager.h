#pragma once
#include "Buffer.h"

class GameObject;

// 인스턴싱 최적화를 위한 렌더링 매니저
// 동일한 메쉬/머티리얼을 공유하는 객체들을 하나의 Draw로 통합
class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	// 프레임 단위로 호출: 모든 오브젝트 렌더링
	void Render(vector<shared_ptr<GameObject>>& gameObjects);

	// 인스턴싱 버퍼 초기화 (프레임 시작 시)
	void ClearBuffer();

	// 완전 초기화 (예: 씬 전환 시)
	void Clear();

private:
	// 특정 인스턴스 ID에 대한 데이터 누적
	void AddParam(uint64 instanceId, const InstancingParams& data);

private:
	// instanceId(메쉬+머티리얼 조합 해시) → 인스턴싱 버퍼 매핑
	map<uint64, shared_ptr<InstancingBuffer>> _buffers;
};
