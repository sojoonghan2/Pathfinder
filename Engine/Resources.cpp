#include "pch.h"
#include "Resources.h"
#include "Engine.h"
#include "MeshData.h"

void Resources::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
}

shared_ptr<Mesh> Resources::LoadPointMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Point");
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(1);
	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(1);
	idx[0] = 0;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Point", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Rectangle", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadPlanMesh()
{
	float width = 2;
	float height = 2;

	std::vector<Vertex> vertices = {
		Vertex(Vec3(-width / 2, 0, -height / 2), Vec2(0, 0), Vec3(0, 1, 0), Vec3(1, 0, 0)),
		Vertex(Vec3(width / 2, 0, -height / 2), Vec2(1, 0), Vec3(0, 1, 0), Vec3(1, 0, 0)),
		Vertex(Vec3(-width / 2, 0, height / 2), Vec2(0, 1), Vec3(0, 1, 0), Vec3(1, 0, 0)),
		Vertex(Vec3(width / 2, 0, height / 2), Vec2(1, 1), Vec3(0, 1, 0), Vec3(1, 0, 0)),
	};

	std::vector<uint32> indices = {
		0, 1, 2,
		1, 3, 2
	};

	shared_ptr<Mesh> planeMesh = make_shared<Mesh>();
	planeMesh->Create(vertices, indices);
	return planeMesh;
}

shared_ptr<Mesh> Resources::LoadWaterMesh()
{
	const int resolution = 100;
	const float width = 2.0f;
	const float height = 2.0f;

	std::vector<Vertex> vertices;
	std::vector<uint32> indices;

	for (int z = 0; z <= resolution; ++z)
	{
		for (int x = 0; x <= resolution; ++x)
		{
			float u = static_cast<float>(x) / resolution;
			float v = static_cast<float>(z) / resolution;
			float px = -width / 2.0f + u * width;
			float pz = -height / 2.0f + v * height;

			Vertex vertex;
			vertex.pos = Vec3(px, 0.0f, pz);
			vertex.uv = Vec2(u, v);
			vertex.normal = Vec3(0, 1, 0);
			vertex.tangent = Vec3(1, 0, 0);
			vertices.push_back(vertex);
		}
	}

	for (int z = 0; z < resolution; ++z)
	{
		for (int x = 0; x < resolution; ++x)
		{
			int i0 = z * (resolution + 1) + x;
			int i1 = i0 + 1;
			int i2 = i0 + (resolution + 1);
			int i3 = i2 + 1;

			// 두 삼각형으로 구성
			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i1);

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	shared_ptr<Mesh> planeMesh = make_shared<Mesh>();
	planeMesh->Create(vertices, indices);
	return planeMesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Cube", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
	if (findMesh)
		return findMesh;

	float radius = 0.5f; // 구의 반지름
	uint32 stackCount = 20; // 가로 분할
	uint32 sliceCount = 20; // 세로 분할

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Sphere", mesh);

	return mesh;
}

shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
	Add(name, texture);

	return texture;
}

shared_ptr<Texture> Resources::CloneRenderTargetTexture(shared_ptr<Texture> originalTexture)
{
	// 1. 원본 텍스처의 속성 가져오기
	D3D12_RESOURCE_DESC desc = originalTexture->GetTex2D()->GetDesc();

	// 2. 복사본 텍스처 생성
	shared_ptr<Texture> cloneTexture = make_shared<Texture>();

	// CD3DX12_RESOURCE_DESC를 사용하여 명시적으로 모든 속성 설정
	D3D12_RESOURCE_DESC newDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		desc.Format,
		desc.Width,
		desc.Height,
		1,                  // arraySize
		desc.MipLevels,    // 원본과 동일한 MipLevels 사용
		1,                  // sampleCount
		0,                  // sampleQuality
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	cloneTexture->Create(
		newDesc.Format,
		static_cast<uint32>(newDesc.Width),
		static_cast<uint32>(newDesc.Height),
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// 3. Source 텍스처 상태 전환
	D3D12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		originalTexture->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier1);

	// 4. Destination 텍스처 상태 전환
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
		cloneTexture->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier2);

	// 5. 텍스처 데이터 복사
	GRAPHICS_CMD_LIST->CopyResource(cloneTexture->GetTex2D().Get(), originalTexture->GetTex2D().Get());

	// 6. 상태 복원: Source 텍스처
	D3D12_RESOURCE_BARRIER barrier3 = CD3DX12_RESOURCE_BARRIER::Transition(
		originalTexture->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier3);

	// 7. 상태 복원: Destination 텍스처
	D3D12_RESOURCE_BARRIER barrier4 = CD3DX12_RESOURCE_BARRIER::Transition(
		cloneTexture->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier4);

	return cloneTexture;
}

shared_ptr<Texture> Resources::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->CreateFromResource(tex2D);
	Add(name, texture);

	return texture;
}

shared_ptr<MeshData> Resources::LoadFBX(const wstring& path)
{
	// FBX 로드
	shared_ptr<MeshData> meshData = MeshData::LoadFromFBX(path);

	// 이름 설정
	meshData->SetName(path + L"_" + std::to_wstring(rand()));

	// 리소스 관리 목록에 추가
	Add(path + L"_" + std::to_wstring(rand()), meshData);

	// meshData 반환
	return meshData;
}

shared_ptr<class MeshData> Resources::LoadBIN(const wstring& path)
{
	wstring key = path;
	shared_ptr<MeshData> meshData = Get<MeshData>(key);
	if (meshData)
		return meshData;
	meshData = MeshData::LoadFromBIN(path);
	meshData->SetName(key);
	Add(key, meshData);

	return meshData;
}

// 셰이더 정보(fx, 어떻게 그려지는지)
void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skybox.fx", info);
		Add<Shader>(L"Skybox", shader);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred", shader);
	}

	// Forward (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"Texture", shader);
	}

	// Occupation (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\Occupation.fx", info);
		Add<Shader>(L"Occupation", shader);
	}

	// PNG(Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\PNG.fx", info, arg);
		Add<Shader>(L"PNG", shader);
	}

	// Billboard
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\Billboard.fx", info, arg);
		Add<Shader>(L"Billboard", shader);
	}

	// UI (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\UI.fx", info, arg);
		Add<Shader>(L"UI", shader);
	}

	// DirLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_DirLight",
			"",
			"",
			"",
			"PS_DirLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"DirLight", shader);
	}

	// PointLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_PointLight",
			"",
			"",
			"",
			"PS_PointLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"PointLight", shader);
	}

	// SpotLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_SpotLight",
			"",
			"",
			"",
			"PS_SpotLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"SpotLight", shader);
	}

	// Final
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_Final",
			"",
			"",
			"",
			"PS_Final"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"Final", shader);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeShader", shader);
	}

	// IceParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\IceParticle.fx", info, arg);
		Add<Shader>(L"IceParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\IceParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"IceComputeParticle", computeShader);
	}

	// CrabParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\CrabParticle.fx", info, arg);
		Add<Shader>(L"CrabParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\CrabParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"CrabComputeParticle", computeShader);
	}

	// GunFlameParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\GunFlameParticle.fx", info, arg);
		Add<Shader>(L"GunFlameParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\GunFlameParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"GunFlameComputeParticle", computeShader);
	}

	// FireParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\FireParticle.fx", info, arg);
		Add<Shader>(L"FireParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\FireParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"FireComputeParticle", computeShader);
	}

	// CataclysmParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\CataclysmParticle.fx", info, arg);
		Add<Shader>(L"CataclysmParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\CataclysmParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"CataclysmComputeParticle", computeShader);
	}

	// RazerParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\RazerParticle.fx", info, arg);
		Add<Shader>(L"RazerParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\RazerParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"RazerComputeParticle", computeShader);
	}

	// OverDriveParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\OverDriveParticle.fx", info, arg);
		Add<Shader>(L"OverDriveParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\OverDriveParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"OverDriveComputeParticle", computeShader);
	}

	// GlitterParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\GlitterParticle.fx", info, arg);
		Add<Shader>(L"GlitterParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\GlitterParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"GlitterComputeParticle", computeShader);
	}

	// LightPillarParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\LightPillarParticle.fx", info, arg);
		Add<Shader>(L"LightPillarParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\LightPillarParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"LightPillarComputeParticle", computeShader);
	}

	// DustParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\DustParticle.fx", info, arg);
		Add<Shader>(L"DustParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\DustParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"DustComputeParticle", computeShader);
	}

	// PortalFrameParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\PortalFrameParticle.fx", info, arg);
		Add<Shader>(L"PortalFrameParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\PortalFrameParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"PortalFrameComputeParticle", computeShader);
	}

	// TestPBRParticle
	{
		ShaderInfo info =
		{
			// 파티클
			SHADER_TYPE::PARTICLE,
			// 뒷면 제거
			RASTERIZER_TYPE::CULL_BACK,
			// 뎁스 버퍼에 기록 X
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			// 알파 블렌딩
			BLEND_TYPE::ALPHA_BLEND,
			// 기본 도형: 점
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\TestPBRParticle.fx", info, arg);
		Add<Shader>(L"TestPBRParticle", shader);

		shared_ptr<Shader> computeShader = make_shared<Shader>();
		computeShader->CreateComputeShader(L"..\\Resources\\Shader\\TestPBRParticle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"TestPBRComputeParticle", computeShader);
	}

	// Shadow
	{
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shadow.fx", info);
		Add<Shader>(L"Shadow", shader);
	}

	// Tessellation
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\tessellation.fx", info, arg);
		Add<Shader>(L"Tessellation", shader);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\animation.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeAnimation", shader);
	}

	// TerrainCube
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_FRONT,
			DEPTH_STENCIL_TYPE::LESS,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\terrainCube.fx", info);
		Add<Shader>(L"TerrainCube", shader);
	}

	// Water
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::ALPHA_BLEND,
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\water.fx", info, arg);
		Add<Shader>(L"Water", shader);
	}

	// Debug
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT
		};

		ShaderArg arg =
		{
			"VS_Debug",
			"",
			"",
			"",
			"PS_Debug"
		};

		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\Debug.fx", info, arg);
		Add<Shader>(L"Debug", shader);
	}
}

// 머터리얼 정보(텍스쳐, 노멀)
void Resources::CreateDefaultMaterial()
{
	// Skybox
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Skybox", material);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DirLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		Add<Material>(L"DirLight", material);
	}

	// PointLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PointLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"PointLight", material);
	}

	// SpotLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"SpotLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"SpotLight", material);
	}

	// Final
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Final");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseLightTarget"));
		material->SetTexture(2, GET_SINGLE(Resources)->Get<Texture>(L"SpecularLightTarget"));
		Add<Material>(L"Final", material);
	}

	// Compute Shader: 데이터를 변환하거나 시뮬레이션을 수행하기 위한 "작업 환경", 일반적인 머터리얼 X
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"ComputeShader", material);
	}

	// IceParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"IceParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"IceParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"IceComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"IceComputeParticle", computeMaterial);
	}

	// CrabParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"CrabParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"CrabParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"CrabComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"CrabComputeParticle", computeMaterial);
	}

	// GunFlameParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"GunFlameParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"GunFlameParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"GunFlameComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"GunFlameComputeParticle", computeMaterial);
	}

	// FireParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"FireParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"FireParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"FireComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"FireComputeParticle", computeMaterial);
	}

	// CataclysmParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"CataclysmParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"CataclysmParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"CataclysmComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"CataclysmComputeParticle", computeMaterial);
	}

	// RazerParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"RazerParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"RazerParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"RazerComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"RazerComputeParticle", computeMaterial);
	}

	// OverDriveParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"OverDriveParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"OverDriveParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"OverDriveComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"OverDriveComputeParticle", computeMaterial);
	}

	// GlitterParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"GlitterParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"GlitterParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"GlitterComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"GlitterComputeParticle", computeMaterial);
	}

	// LightPillarParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"LightPillarParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"LightPillarParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"LightPillarComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"LightPillarComputeParticle", computeMaterial);
	}

	// DustParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DustParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"DustParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"DustComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"DustComputeParticle", computeMaterial);
	}

	// PortalFrameParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PortalFrameParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"PortalFrameParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"PortalFrameComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"PortalFrameComputeParticle", computeMaterial);
	}

	// TestPBRParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TestPBRParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"TestPBRParticle", material);

		shared_ptr<Shader> computShader = GET_SINGLE(Resources)->Get<Shader>(L"TestPBRComputeParticle");
		shared_ptr<Material> computeMaterial = make_shared<Material>();
		computeMaterial->SetShader(computShader);
		Add<Material>(L"TestPBRComputeParticle", computeMaterial);
	}

	// GameObject
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"GameObject", material);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Shadow");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Shadow", material);
	}

	// Tessellation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Tessellation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Tessellation", material);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeAnimation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeAnimation", material);
	}

	// TerrainCube
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TerrainCube");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"TerrainCube", material);
	}

	// Water
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Water");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Water", L"..\\Resources\\Texture\\Water.jpeg");
		shared_ptr<Texture> normalTexture = GET_SINGLE(Resources)->Load<Texture>(L"Water_Normal", L"..\\Resources\\Texture\\Water_Normal.jpg");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetTexture(1, normalTexture);

		Add<Material>(L"Water", material);
	}

	// Debug
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Debug");

		shared_ptr<Material> material = make_shared<Material>();
		material = std::make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"Debug", material);
	}
}