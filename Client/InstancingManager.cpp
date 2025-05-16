#include "pch.h"
#include "InstancingManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
	unordered_map<uint64, vector<shared_ptr<GameObject>>> instanceGroups;

	// 인스턴스 ID 기준으로 그룹화
	for (const auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		instanceGroups[instanceId].push_back(gameObject);
	}

	// 각 그룹별로 렌더링
	for (auto& [instanceId, group] : instanceGroups)
	{
		if (group.size() == 1)
		{
			// 단일 렌더링
			group[0]->GetMeshRenderer()->Render();
		}
		else
		{
			// 인스턴싱 렌더링 준비
			for (const auto& gameObject : group)
			{
				InstancingParams params;
				params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
				params.matWV = params.matWorld * Camera::S_MatView;
				params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

				AddParam(instanceId, params);
			}

			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			group[0]->GetMeshRenderer()->Render(buffer);
		}
	}
}

void InstancingManager::ClearBuffer()
{
	for (auto& [instanceId, buffer] : _buffers)
	{
		if (buffer)
			buffer->Clear();
	}
}

void InstancingManager::AddParam(uint64 instanceId, const InstancingParams& data)
{
	if (_buffers.find(instanceId) == _buffers.end())
		_buffers[instanceId] = make_shared<InstancingBuffer>();

	_buffers[instanceId]->AddData(data);
}