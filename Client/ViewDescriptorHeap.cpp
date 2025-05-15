#include "pch.h"
#include "ViewDescriptorHeap.h"
#include "GameFramework.h"

namespace
{
	// 컴파일 타임에 값이 결정
	constexpr uint32 GRAPHICS_ROOT_DESCRIPTOR_SLOT = 1;
	constexpr uint32 COMPUTE_ROOT_DESCRIPTOR_SLOT = 0;

	inline void CopyDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE src, D3D12_CPU_DESCRIPTOR_HANDLE dest)
	{
		constexpr uint32 range = 1;
		DEVICE->CopyDescriptors(1, &dest, &range, 1, &src, &range, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void GraphicsViewDescriptorHeap::Init(uint32 count)
{
	_groupCount = count;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = count * (CBV_SRV_REGISTER_COUNT - 1); // b0는 전역
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_groupSize = _handleSize * (CBV_SRV_REGISTER_COUNT - 1); // b0는 전역
}

void GraphicsViewDescriptorHeap::Clear()
{
	_currentGroupIndex = 0;
}

void GraphicsViewDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	CopyDescriptor(srcHandle, GetCPUHandle(reg));
}

void GraphicsViewDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	CopyDescriptor(srcHandle, GetCPUHandle(reg));
}

void GraphicsViewDescriptorHeap::CommitTable()
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	GRAPHICS_CMD_LIST->SetGraphicsRootDescriptorTable(GRAPHICS_ROOT_DESCRIPTOR_SLOT, handle);
	_currentGroupIndex++;
}

template<typename T>
D3D12_CPU_DESCRIPTOR_HANDLE GraphicsViewDescriptorHeap::GetCPUHandle(T reg)
{
	return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsViewDescriptorHeap::GetCPUHandle(uint8 reg)
{
	assert(reg > 0); // b0는 글로벌, 그룹 내에서는 b1부터 사용
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	handle.ptr += (reg - 1) * _handleSize;
	return handle;
}

void ComputeViewDescriptorHeap::Init()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = TOTAL_REGISTER_COUNT;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeViewDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	CopyDescriptor(srcHandle, GetCPUHandle(reg));
}

void ComputeViewDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	CopyDescriptor(srcHandle, GetCPUHandle(reg));
}

void ComputeViewDescriptorHeap::SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg)
{
	CopyDescriptor(srcHandle, GetCPUHandle(reg));
}

void ComputeViewDescriptorHeap::CommitTable()
{
	ID3D12DescriptorHeap* descHeap = _descHeap.Get();
	COMPUTE_CMD_LIST->SetDescriptorHeaps(1, &descHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = descHeap->GetGPUDescriptorHandleForHeapStart();
	COMPUTE_CMD_LIST->SetComputeRootDescriptorTable(COMPUTE_ROOT_DESCRIPTOR_SLOT, handle);
}

template<typename T>
D3D12_CPU_DESCRIPTOR_HANDLE ComputeViewDescriptorHeap::GetCPUHandle(T reg)
{
	return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeViewDescriptorHeap::GetCPUHandle(uint8 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += reg * _handleSize;
	return handle;
}