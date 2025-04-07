#pragma once
#include "Object.h"

enum class SHADER_TYPE : uint8
{
	DEFERRED, // 지연 렌더링, 다중 렌더 타겟 사용(셰이더에서 구조체 리턴 가능)
	FORWARD, // 일반 전방 렌더링, 단일 렌더 타겟 사용(셰이더에서 구조체 리턴 불가능)
	LIGHTING,
	PARTICLE,
	COMPUTE,
	SHADOW,
};

// *****************************************************************************************
// struct PS_OUT
// {
//	 float4 position : SV_Target0;
//	 float4 normal : SV_Target1;
//	 float4 color : SV_Target2;
// };
// 이런식으로 SV_Targetn을 사용하여 다중 렌더 타겟을 사용 가능함
// 만약 FORWARD 렌더링의 경우에는 이런식으로 다중 렌더 타겟을 사용이 불가능하기 떄문에
// return output 처럼 구조체 리턴이 불가능하고 return color 같이 float4 형만 리턴이 가능함
// *****************************************************************************************

enum class RASTERIZER_TYPE : uint8
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST, // 깊이 테스트(X) + 깊이 기록(O)
	NO_DEPTH_TEST_NO_WRITE, // 깊이 테스트(X) + 깊이 기록(X)
	LESS_NO_WRITE, // 깊이 테스트(O) + 깊이 기록(X)
};

enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLEND,
	ONE_TO_ONE_BLEND,
	END,
};

struct ShaderInfo
{
	SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

struct ShaderArg
{
	const string vs = "VS_Main";
	const string hs;
	const string ds;
	const string gs;
	const string ps = "PS_Main";
};

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void CreateGraphicsShader(const wstring& path, ShaderInfo info = ShaderInfo(), ShaderArg arg = ShaderArg());
	void CreateComputeShader(const wstring& path, const string& name, const string& version);
	
	void Update();

	SHADER_TYPE GetShaderType() { return _info.shaderType; }

	static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(D3D_PRIMITIVE_TOPOLOGY topology);

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreateHullShader(const wstring& path, const string& name, const string& version);
	void CreateDomainShader(const wstring& path, const string& name, const string& version);
	void CreateGeometryShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
	ShaderInfo _info;
	ComPtr<ID3D12PipelineState>			_pipelineState;

	// GraphicsShader
	ComPtr<ID3DBlob>					_vsBlob;
	ComPtr<ID3DBlob>					_hsBlob;
	ComPtr<ID3DBlob>					_dsBlob;	
	ComPtr<ID3DBlob>					_gsBlob;
	ComPtr<ID3DBlob>					_psBlob;
	ComPtr<ID3DBlob>					_errBlob;	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  _graphicsPipelineDesc = {};

	// ComputeShader
	ComPtr<ID3DBlob>					_csBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC   _computePipelineDesc = {};
};

