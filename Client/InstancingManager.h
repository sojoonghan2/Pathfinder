#pragma once
#include "Buffer.h"

class GameObject;

class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	void Render(vector<shared_ptr<GameObject>>& gameObjects);

	void ClearBuffer();
	void Clear() { _buffers.clear(); }

private:
	// 특정 인스턴스 ID에 대한 파라미터 추가
	void AddParam(uint64 instanceId, InstancingParams& data);

private:
	// 각 인스턴스 ID에 대해 관련된 인스턴싱 버퍼를 저장하는 맵
	map<uint64/*instanceId*/, shared_ptr<InstancingBuffer>> _buffers;
};
