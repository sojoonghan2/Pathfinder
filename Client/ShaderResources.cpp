#include "pch.h"
#include "ShaderResources.h"
#include "Resources.h"
#include "GameFramework.h"
#include "MeshData.h"

void ShaderResources::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
}

// 셰이더 정보(어떻게 그려지는지)
void ShaderResources::CreateDefaultShader()
{
	auto createShader = [this](const wstring& name, const wstring& path, const ShaderInfo& info,
		const ShaderArg& arg = {}) {
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader->CreateGraphicsShader(path, info, arg);
			GET_SINGLE(Resources)->Add<Shader>(name, shader);
			return shader;
		};

	auto createComputeShader = [this](const wstring& name, const wstring& path,
		const string& entry = "CS_Main", const string& version = "cs_5_0") {
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader->CreateComputeShader(path, entry, version);
			GET_SINGLE(Resources)->Add<Shader>(name, shader);
			return shader;
		};

	auto createParticleShaders = [&](const wstring& baseName, const wstring& path) {
		ShaderInfo info = {
			SHADER_TYPE::PARTICLE,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg = {
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		createShader(baseName + L"Particle", path + L"Particle.hlsl", info, arg);
		createComputeShader(baseName + L"ComputeParticle", path + L"Particle.hlsl");
		};

	// Skybox
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		createShader(L"Skybox", L"..\\Resources\\Shader\\skybox.hlsl", info);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info = {
			SHADER_TYPE::DEFERRED
		};

		createShader(L"Deferred", L"..\\Resources\\Shader\\deferred.hlsl", info);
	}

	// Forward (Forward)
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
		};

		createShader(L"Forward", L"..\\Resources\\Shader\\forward.hlsl", info);
	}

	// Texture (Forward)
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
		};

		ShaderArg arg = {
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		createShader(L"Texture", L"..\\Resources\\Shader\\forward.hlsl", info, arg);
	}

	// Occupation (Deferred)
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::ALPHA_BLEND
		};

		createShader(L"Occupation", L"..\\Resources\\Shader\\Occupation.hlsl", info);
	}

	// SkillIcon(Forward)
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg = {
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		createShader(L"SkillIcon", L"..\\Resources\\Shader\\SkillIcon.hlsl", info, arg);
	}

	// Billboard
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg = {
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		createShader(L"Billboard", L"..\\Resources\\Shader\\Billboard.hlsl", info, arg);
	}

	// UI (Forward)
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg = {
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		createShader(L"UI", L"..\\Resources\\Shader\\UI.hlsl", info, arg);
	}

	// DirLight
	{
		ShaderInfo info = {
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg = {
			"VS_DirLight",
			"",
			"",
			"",
			"PS_DirLight"
		};

		createShader(L"DirLight", L"..\\Resources\\Shader\\lighting.hlsl", info, arg);
	}

	// PointLight
	{
		ShaderInfo info = {
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_FRONT,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg = {
			"VS_PointLight",
			"",
			"",
			"",
			"PS_PointLight"
		};

		createShader(L"PointLight", L"..\\Resources\\Shader\\lighting.hlsl", info, arg);
	}

	// SpotLight
	{
		ShaderInfo info = {
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg = {
			"VS_SpotLight",
			"",
			"",
			"",
			"PS_SpotLight"
		};

		createShader(L"SpotLight", L"..\\Resources\\Shader\\lighting.hlsl", info, arg);
	}

	// Final
	{
		ShaderInfo info = {
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg = {
			"VS_Final",
			"",
			"",
			"",
			"PS_Final"
		};

		createShader(L"Final", L"..\\Resources\\Shader\\lighting.hlsl", info, arg);
	}

	// Compute Shader
	{
		createComputeShader(L"ComputeShader", L"..\\Resources\\Shader\\compute.hlsl");
	}

	// Shadow
	{
		ShaderInfo info = {
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
		};

		createShader(L"Shadow", L"..\\Resources\\Shader\\shadow.hlsl", info);
	}

	// Tessellation
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg = {
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		createShader(L"Tessellation", L"..\\Resources\\Shader\\tessellation.hlsl", info, arg);
	}

	// Animation
	{
		createComputeShader(L"ComputeAnimation", L"..\\Resources\\Shader\\animation.hlsl");
	}

	// TerrainCube
	{
		ShaderInfo info = {
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_FRONT,
			DEPTH_STENCIL_TYPE::LESS,
		};

		createShader(L"TerrainCube", L"..\\Resources\\Shader\\terrainCube.hlsl", info);
	}

	// Water
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::ALPHA_BLEND,
		};

		ShaderArg arg = {
			"VS_Main",
			"",
			"",
			"",
			"PS_Main",
		};

		createShader(L"Water", L"..\\Resources\\Shader\\water.hlsl", info, arg);
	}

	// Debug
	{
		ShaderInfo info = {
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT
		};

		ShaderArg arg = {
			"VS_Debug",
			"",
			"",
			"",
			"PS_Debug"
		};

		createShader(L"Debug", L"..\\Resources\\Shader\\Debug.hlsl", info, arg);
	}

	createParticleShaders(L"Ice", L"..\\Resources\\Shader\\Ice");
	createParticleShaders(L"Crab", L"..\\Resources\\Shader\\Crab");
	createParticleShaders(L"GunFlame", L"..\\Resources\\Shader\\GunFlame");
	createParticleShaders(L"Fire", L"..\\Resources\\Shader\\Fire");
	createParticleShaders(L"Cataclysm", L"..\\Resources\\Shader\\Cataclysm");
	createParticleShaders(L"Razer", L"..\\Resources\\Shader\\Razer");
	createParticleShaders(L"OverDrive", L"..\\Resources\\Shader\\OverDrive");
	createParticleShaders(L"Glitter", L"..\\Resources\\Shader\\Glitter");
	createParticleShaders(L"LightPillar", L"..\\Resources\\Shader\\LightPillar");
	createParticleShaders(L"Dust", L"..\\Resources\\Shader\\Dust");
	createParticleShaders(L"PortalFrame", L"..\\Resources\\Shader\\PortalFrame");
	createParticleShaders(L"TestPBR", L"..\\Resources\\Shader\\TestPBR");
}

// 머터리얼 정보(텍스쳐, 노멀)
void ShaderResources::CreateDefaultMaterial()
{
	// 기본 머터리얼 생성 람다 함수
	auto createMaterial = [&](const wstring& name, const wstring& shaderName) -> shared_ptr<Material> {
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(shaderName);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		GET_SINGLE(Resources)->Add<Material>(name, material);
		return material;
		};

	// 파티클 머터리얼 및 컴퓨트 셰이더 페어 생성 람다 함수
	auto createParticlePair = [&](const wstring& baseName) {
		// 렌더링용 파티클 머터리얼
		createMaterial(baseName + L"Particle", baseName + L"Particle");

		// 컴퓨트 셰이더용 파티클 머터리얼
		createMaterial(baseName + L"ComputeParticle", baseName + L"ComputeParticle");
		};

	// Skybox
	createMaterial(L"Skybox", L"Skybox");

	// DirLight
	{
		shared_ptr<Material> material = createMaterial(L"DirLight", L"DirLight");
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
	}

	// PointLight
	{
		const WindowInfo& window = GFramework->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Material> material = createMaterial(L"PointLight", L"PointLight");
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
	}

	// SpotLight
	{
		const WindowInfo& window = GFramework->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Material> material = createMaterial(L"SpotLight", L"SpotLight");
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
	}

	// Final(디퍼드 렌더링 최종 출력용 머터리얼)
	{
		shared_ptr<Material> material = createMaterial(L"Final", L"Final");
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseLightTarget"));
		material->SetTexture(2, GET_SINGLE(Resources)->Get<Texture>(L"SpecularLightTarget"));
	}

	// Compute Shader
	createMaterial(L"ComputeShader", L"ComputeShader");

	// GameObject
	createMaterial(L"GameObject", L"Deferred");

	// Shadow
	createMaterial(L"Shadow", L"Shadow");

	// Tessellation
	createMaterial(L"Tessellation", L"Tessellation");

	// ComputeAnimation
	createMaterial(L"ComputeAnimation", L"ComputeAnimation");

	// TerrainCube
	createMaterial(L"TerrainCube", L"TerrainCube");

	// Water
	{
		shared_ptr<Material> material = createMaterial(L"Water", L"Water");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Water", L"..\\Resources\\Texture\\Water.jpeg");
		shared_ptr<Texture> normalTexture = GET_SINGLE(Resources)->Load<Texture>(L"Water_Normal", L"..\\Resources\\Texture\\Water_Normal.jpg");
		material->SetTexture(0, texture);
		material->SetTexture(1, normalTexture);
	}

	// Debug
	createMaterial(L"Debug", L"Debug");

	// Particle
	createParticlePair(L"Ice");
	createParticlePair(L"Crab");
	createParticlePair(L"GunFlame");
	createParticlePair(L"Fire");
	createParticlePair(L"Cataclysm");
	createParticlePair(L"Razer");
	createParticlePair(L"OverDrive");
	createParticlePair(L"Glitter");
	createParticlePair(L"LightPillar");
	createParticlePair(L"Dust");
	createParticlePair(L"PortalFrame");
	createParticlePair(L"TestPBR");
}