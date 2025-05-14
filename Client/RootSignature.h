#pragma once

// 그래픽스 및 컴퓨트 파이프라인용 루트 시그니처를 생성 및 관리하는 클래스
// CBV, SRV, UAV, Sampler 바인딩을 정의하며, 각 파이프라인에 맞는 루트 시그니처를 분리 보관
class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return _computeRootSignature; }

private:
	void InitStaticSamplers();
	ComPtr<ID3D12RootSignature> CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	D3D12_STATIC_SAMPLER_DESC	_samplerDesc[2];
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;
	ComPtr<ID3D12RootSignature>	_computeRootSignature;
};
