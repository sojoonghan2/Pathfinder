#include <d3d12.h>

#include "pch.h"
#include "Mesh.h"
#include "Application.h"
#include "Material.h"
#include "Buffer.h"
#include "FBXLoader.h"
#include "BINLoader.h"
#include "Buffer.h"

////////////////////////////////////////////////////////////////////////// VRS �۾� �� //////////////////////////////////////////////////////////////////////////
ComPtr<ID3D12Resource> _shadingRateImage;

void Mesh::CreateVRSImage(UINT width, UINT height)
{
	if (!_supportsVRS)
		return;

	D3D12_RESOURCE_DESC vrsDesc = {};
	vrsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	vrsDesc.Width = width / 16;   // VRS ���� 16x16 ��� ����
	vrsDesc.Height = height / 16;
	vrsDesc.DepthOrArraySize = 1;
	vrsDesc.MipLevels = 1;
	vrsDesc.Format = DXGI_FORMAT_R8_UINT;
	vrsDesc.SampleDesc.Count = 1;
	vrsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	vrsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hr = DEVICE->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&vrsDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_shadingRateImage)
	);

	if (FAILED(hr))
	{
		printf("VRS �� ���� ����!\n");
	}
}

void Mesh::CreateVRSUploadBuffer(UINT width, UINT height)
{
	// ���� �⺻ VRS ���� ���� Ȯ��
	if (!_supportsVRS)
		return;

	// GPU�� Tier 2 VRS�� �����ϴ��� ��Ÿ�ӿ� Ȯ��
	D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
	if (FAILED(DEVICE->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS6,
		&options6,
		sizeof(options6))))
	{
		cout << "qurry failed" << endl;
		// ���� ���� �� ���ε� ���� �������� ����
		return;
	}
	if (options6.VariableShadingRateTier < D3D12_VARIABLE_SHADING_RATE_TIER_2)
	{
		cout << "Not supported" << endl;
		// Tier 2 ������ �� ���ε� ���� �������� ����
		return;
	}

	// tileWidth, tileHeight ������ ���� ũ�� ���
	UINT vrsWidth = width / 16;
	UINT vrsHeight = height / 16;
	UINT bufferSize = vrsWidth * vrsHeight * sizeof(UINT8);

	D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	HRESULT hr = DEVICE->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vrsUploadBuffer)
	);

	if (FAILED(hr))
	{
		printf("VRS ���ε� ���� ���� ����! HRESULT=0x%08X\n", hr);
	}
}

void Mesh::UploadVRSData()
{
	if (!_supportsVRS || _shadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_2)
		return;

	UINT width = _shadingRateImage->GetDesc().Width;
	UINT height = _shadingRateImage->GetDesc().Height;

	UINT8* vrsData = new UINT8[width * height];

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
				vrsData[y * width + x] = shading_rate;  // �⺻ ���̵� ����Ʈ
		}
	}

	// ���ε� ���ۿ� ������ ����
	void* mappedMemory;
	_vrsUploadBuffer->Map(0, nullptr, &mappedMemory);
	memcpy(mappedMemory, vrsData, width * height * sizeof(UINT8));
	_vrsUploadBuffer->Unmap(0, nullptr);

	delete[] vrsData;

	// VRS ���� GPU�� ����
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_shadingRateImage.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier);

	GRAPHICS_CMD_LIST->CopyResource(_shadingRateImage.Get(), _vrsUploadBuffer.Get());

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_shadingRateImage.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier);
}

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
	if (SUCCEEDED(DEVICE->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6))))
	{
		_supportsVRS = (options6.VariableShadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED);
		_shadingRateTier = options6.VariableShadingRateTier;
	}
}

//////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{
}

void Mesh::Create(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{

	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);

	UINT width = static_cast<float>(P_Application->GetWindow().width);
	UINT height = static_cast<float>(P_Application->GetWindow().height);

	if (_supportsVRS)
	{
		if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2)
		{
			CreateVRSImage(width, height);
			CreateVRSUploadBuffer(width, height);
			UploadVRSData();
		}
		// Tier 1�� ���� �̹��� �ʿ� ����
	}
}

void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	// ���� �� �ε��� ���� ����
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	P_Application->GetGraphicsDescHeap()->CommitTable();

	
	/////////////////////////////// VRS /////////////////////////////////////
	if (_supportsVRS)
	{
		ComPtr<ID3D12GraphicsCommandList5> commandList5;
		GRAPHICS_CMD_LIST->QueryInterface(IID_PPV_ARGS(&commandList5));

		if (commandList5)
		{
			if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2 && _shadingRateImage)
			{
				commandList5->RSSetShadingRateImage(_shadingRateImage.Get());
			}
			else if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_1)
			{
				// ����: 2x2 ���̵� ����Ʈ (BLOCK_2X2)
				commandList5->RSSetShadingRate(D3D12_SHADING_RATE_2X2, nullptr);
			}
		}
	}

	// ù ��° ��ο� ȣ�� (VRS ����� ����)
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);

	// VRS ���� ���� (���̵� ����Ʈ �̹��� ����)
	if (_supportsVRS)
	{
		ComPtr<ID3D12GraphicsCommandList5> commandList5;
		GRAPHICS_CMD_LIST->QueryInterface(IID_PPV_ARGS(&commandList5));

		if (commandList5)
		{
			if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2)
			{
				commandList5->RSSetShadingRateImage(nullptr);
			}
			else if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_1)
			{
				commandList5->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr); // ������� ����
			}
		}
	}
	/////////////////////////////// VRS /////////////////////////////////////
	
	// �� ��° ��ο� ȣ�� (VRS ������ ����)
	//GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);
	
}

void Mesh::Render(shared_ptr<InstancingBuffer>& buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	P_Application->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, buffer->GetCount(), 0, 0, 0);
}

shared_ptr<Mesh> Mesh::CreateFromFBX(const FbxMeshInfo* meshInfo, FBXLoader& loader)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			vector<uint32> defaultBuffer{ 0 };
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			mesh->CreateIndexBuffer(buffer);
		}
	}

	if (meshInfo->hasAnimation)
		mesh->CreateBonesAndAnimations(loader);

	return mesh;
}

shared_ptr<Mesh> Mesh::CreateFromBIN(const BINInfo* meshInfo, shared_ptr<BINLoader> loader)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			vector<uint32> defaultBuffer{ 0 };
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			mesh->CreateIndexBuffer(buffer);
		}
	}

	return mesh;
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	// 1. ���� ��� ���� ���
	if (buffer.empty())
	{
		wcout << L"[error] CreateVertexBuffer: Input vertexBuffer Empty" << endl;
		return;
	}

	// 2. ���� ũ�� ���
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// 3. GPU �ڿ� ����
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	HRESULT hr = DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	if (FAILED(hr) || _vertexBuffer == nullptr)
	{
		wcout << L"[error] VertexBuffer create fail. bufferSize: " << bufferSize << L" bytes" << endl;
		return;
	}

	// 4. ������ ����
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	if (SUCCEEDED(_vertexBuffer->Map(0, &readRange, &vertexDataBuffer)) && vertexDataBuffer)
	{
		::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
		_vertexBuffer->Unmap(0, nullptr);
	}
	else
	{
		wcout << L"[error] VertexBuffer Map fail" << endl;
		return;
	}

	// 5. View �ʱ�ȭ
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex);
	_vertexBufferView.SizeInBytes = bufferSize;
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	uint32 indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	HRESULT hr = DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW	indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		indexCount
	};

	_vecIndexInfo.push_back(info);
}

void Mesh::CreateBonesAndAnimations(class FBXLoader& loader)
{
#pragma region AnimClip
	uint32 frameCount = 0;
	vector<shared_ptr<FbxAnimClipInfo>>& animClips = loader.GetAnimClip();
	for (shared_ptr<FbxAnimClipInfo>& ac : animClips)
	{
		AnimClipInfo info = {};

		info.animName = ac->name;
		info.duration = ac->endTime.GetSecondDouble() - ac->startTime.GetSecondDouble();

		int32 startFrame = static_cast<int32>(ac->startTime.GetFrameCount(ac->mode));
		int32 endFrame = static_cast<int32>(ac->endTime.GetFrameCount(ac->mode));
		info.frameCount = endFrame - startFrame;

		info.keyFrames.resize(ac->keyFrames.size());

		const int32 boneCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 b = 0; b < boneCount; b++)
		{
			auto& vec = ac->keyFrames[b];

			const int32 size = static_cast<int32>(vec.size());
			frameCount = max(frameCount, static_cast<uint32>(size));
			info.keyFrames[b].resize(size);

			for (int32 f = 0; f < size; f++)
			{
				FbxKeyFrameInfo& kf = vec[f];
				// FBX?�서 ?�싱???�보?�로 채워준??
				KeyFrameInfo& kfInfo = info.keyFrames[b][f];
				kfInfo.time = kf.time;
				kfInfo.frame = static_cast<int32>(size);
				kfInfo.scale.x = static_cast<float>(kf.matTransform.GetS().mData[0]);
				kfInfo.scale.y = static_cast<float>(kf.matTransform.GetS().mData[1]);
				kfInfo.scale.z = static_cast<float>(kf.matTransform.GetS().mData[2]);
				kfInfo.rotation.x = static_cast<float>(kf.matTransform.GetQ().mData[0]);
				kfInfo.rotation.y = static_cast<float>(kf.matTransform.GetQ().mData[1]);
				kfInfo.rotation.z = static_cast<float>(kf.matTransform.GetQ().mData[2]);
				kfInfo.rotation.w = static_cast<float>(kf.matTransform.GetQ().mData[3]);
				kfInfo.translate.x = static_cast<float>(kf.matTransform.GetT().mData[0]);
				kfInfo.translate.y = static_cast<float>(kf.matTransform.GetT().mData[1]);
				kfInfo.translate.z = static_cast<float>(kf.matTransform.GetT().mData[2]);
			}
		}

		_animClips.push_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<shared_ptr<FbxBoneInfo>>& bones = loader.GetBones();
	for (shared_ptr<FbxBoneInfo>& bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.parentIndex = bone->parentIndex;
		boneInfo.matOffset = GetMatrix(bone->matOffset);
		boneInfo.boneName = bone->boneName;
		_bones.push_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh())
	{
		// BoneOffet ?�렬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		_offsetBuffer = make_shared<Buffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo& animClip = _animClips[i];

			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < min(keyFrameCount, animClip.frameCount); f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams
					{
						Vec4(animClip.keyFrames[b][f].scale),
						animClip.keyFrames[b][f].rotation, // Quaternion
						Vec4(animClip.keyFrames[b][f].translate)
					};
				}
			}

			// StructuredBuffer ?�팅
			_frameBuffer.push_back(make_shared<Buffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()), frameParams.data());
		}
	}
#pragma endregion
}

Matrix Mesh::GetMatrix(FbxAMatrix& matrix)
{
	Matrix mat;

	for (int32 y = 0; y < 4; ++y)
		for (int32 x = 0; x < 4; ++x)
			mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

	return mat;
}