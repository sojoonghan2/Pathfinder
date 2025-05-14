#include "pch.h"
#include "RootSignature.h"
#include "Application.h"

void RootSignature::Init()
{
	InitStaticSamplers();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();
}

void RootSignature::InitStaticSamplers()
{
	_samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	_samplerDesc[1] = CD3DX12_STATIC_SAMPLER_DESC(1);
}

void RootSignature::CreateGraphicsRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[] = {
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT - 1, 1), // b1 ~
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0),    // t0 ~
	};

	CD3DX12_ROOT_PARAMETER params[2];
	params[0].InitAsConstantBufferView(static_cast<uint32>(CBV_REGISTER::b0)); // b0
	params[1].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(params), params, _countof(_samplerDesc), _samplerDesc);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	_graphicsRootSignature = CreateRootSignature(sigDesc);
}

void RootSignature::CreateComputeRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[] = {
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_REGISTER_COUNT, 0),
	};

	CD3DX12_ROOT_PARAMETER params[1];
	params[0].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(params), params, _countof(_samplerDesc), _samplerDesc);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	_computeRootSignature = CreateRootSignature(sigDesc);

	COMPUTE_CMD_LIST->SetComputeRootSignature(_computeRootSignature.Get());
}

ComPtr<ID3D12RootSignature> RootSignature::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	HRESULT hr = ::D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);

	if (FAILED(hr))
	{
		if (blobError)
		{
			OutputDebugStringA(static_cast<const char*>(blobError->GetBufferPointer()));
		}
		throw std::runtime_error("Failed to serialize root signature");
	}

	ComPtr<ID3D12RootSignature> rootSignature;
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	return rootSignature;
}