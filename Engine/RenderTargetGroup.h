#pragma once
#include "Texture.h"

// ���� Ÿ�� �׷�
enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN, // BACK_BUFFER, FRONT_BUFFER
	SHADOW, // SHADOW
	G_BUFFER, // POSITION, NORMAL, COLOR
	LIGHTING, // DIFFUSE LIGHT, SPECULAR LIGHT	
	END,
};

// ��ü ���� Ÿ�� ����
enum
{
	RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

// �� ���� Ÿ�ٿ� ���� ������ Ŭ���� ����
struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4];
};

class RenderTargetGroup
{
public:
	// ���� Ÿ�� �׷� ����
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);

	// ������ �ε������� ���� Ÿ�ٵ��� OM �ܰ迡 ����
	void OMSetRenderTargets(uint32 count, uint32 offset);
	void OMSetRenderTargets();

	// Ư�� �ε����� �ش��ϴ� ���� Ÿ���� Ŭ�����ϰų� ��ü ���� Ÿ�� Ŭ����
	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetView();

	shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() { return _dsTexture; }

	// ���ҽ��� ���� Ÿ�� �� ���� ��ȯ�� ���� GPU�� ����ȭ ��û
	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	uint32							_rtCount;
	shared_ptr<Texture>				_dsTexture;
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;

private:
	uint32							_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE		_dsvHeapBegin;

private:
	D3D12_RESOURCE_BARRIER			_targetToResource[8];
	D3D12_RESOURCE_BARRIER			_resourceToTarget[8];
};
