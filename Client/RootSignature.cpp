#include "pch.h"
#include "RootSignature.h"
#include "GameFramework.h"

void RootSignature::Init()
{
	InitStaticSamplers();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();
}

void RootSignature::InitStaticSamplers()
{
	for (int i = 0; i < 2; ++i)
		_samplerDesc[i] = CD3DX12_STATIC_SAMPLER_DESC(i);
}

void RootSignature::CreateGraphicsRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT - 1, 1); // b1 ~
	CD3DX12_DESCRIPTOR_RANGE srvTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0);     // t0 ~

	array<CD3DX12_DESCRIPTOR_RANGE, 2> ranges = { cbvTable, srvTable };

	CD3DX12_ROOT_PARAMETER rootParams[2];
	rootParams[0].InitAsConstantBufferView(static_cast<UINT>(CBV_REGISTER::b0)); // b0
	rootParams[1].InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data());

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = _countof(rootParams);
	desc.pParameters = rootParams;
	desc.NumStaticSamplers = _countof(_samplerDesc);
	desc.pStaticSamplers = _samplerDesc;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	_graphicsRootSignature = CreateRootSignature(desc);
}

void RootSignature::CreateComputeRootSignature()
{
	array<CD3DX12_DESCRIPTOR_RANGE, 3> ranges = {
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_REGISTER_COUNT, 0),
	};

	array<CD3DX12_ROOT_PARAMETER, 1> params = {};
	params[0].InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data());

	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = static_cast<UINT>(params.size());
	sigDesc.pParameters = params.data();
	sigDesc.NumStaticSamplers = _countof(_samplerDesc);
	sigDesc.pStaticSamplers = _samplerDesc;
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	_computeRootSignature = CreateRootSignature(sigDesc);

	COMPUTE_CMD_LIST->SetComputeRootSignature(_computeRootSignature.Get());
}

ComPtr<ID3D12RootSignature> RootSignature::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	HRESULT hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);

	if (FAILED(hr))
	{
		if (blobError)
			OutputDebugStringA(static_cast<const char*>(blobError->GetBufferPointer()));

		throw std::runtime_error("Failed to serialize root signature");
	}

	ComPtr<ID3D12RootSignature> rootSignature;
	DEVICE->CreateRootSignature(
		0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));

	return rootSignature;
}