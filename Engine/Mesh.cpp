#include <d3d12.h>

#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"
#include "FBXLoader.h"
#include "BINLoader.h"
#include "StructuredBuffer.h"

////////////////////////////////////////////////////////////////////////// VRS ÀÛ¾÷ Áß //////////////////////////////////////////////////////////////////////////
ComPtr<ID3D12Resource> _shadingRateImage;

void Mesh::CreateVRSImage(UINT width, UINT height)
{
	if (!_supportsVRS)
		return;

	D3D12_RESOURCE_DESC vrsDesc = {};
	vrsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	vrsDesc.Width = width / 16;   // VRS ¸ÊÀº 16x16 ºí·Ï ´ÜÀ§
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
		printf("VRS ¸Ê »ý¼º ½ÇÆÐ!\n");
	}
}

void Mesh::CreateVRSUploadBuffer(UINT width, UINT height)
{
	// ¸ÕÀú ±âº» VRS Áö¿ø ¿©ºÎ È®ÀÎ
	if (!_supportsVRS)
		return;

	// GPU°¡ Tier 2 VRS¸¦ Áö¿øÇÏ´ÂÁö ·±Å¸ÀÓ¿¡ È®ÀÎ
	D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
	if (FAILED(DEVICE->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS6,
		&options6,
		sizeof(options6))))
	{
		cout << "qurry failed" << endl;
		// Äõ¸® ½ÇÆÐ ½Ã ¾÷·Îµå ¹öÆÛ »ý¼ºÇÏÁö ¾ÊÀ½
		return;
	}

	// tileWidth, tileHeight ´ÜÀ§·Î ¹öÆÛ Å©±â °è»ê
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
		printf("VRS ¾÷·Îµå ¹öÆÛ »ý¼º ½ÇÆÐ! HRESULT=0x%08X\n", hr);
	}
}

void Mesh::UploadVRSData()
{
	if (!_supportsVRS)
		return;

	UINT width = _shadingRateImage->GetDesc().Width;
	UINT height = _shadingRateImage->GetDesc().Height;
	UINT pitch = width; // R8_UINT, 1 byte per texel

	// 1. VRS µ¥ÀÌÅÍ ÁØºñ
	std::vector<UINT8> vrsData(width * height, shading_rate_size);

	// 2. ¾÷·Îµå ¹öÆÛ¿¡ º¹»ç
	void* mappedMemory = nullptr;
	_vrsUploadBuffer->Map(0, nullptr, &mappedMemory);
	memcpy(mappedMemory, vrsData.data(), width * height * sizeof(UINT8));
	_vrsUploadBuffer->Unmap(0, nullptr);

	// 3. ¼­ºê¸®¼Ò½º footprint ¼³Á¤
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	footprint.Offset = 0;
	footprint.Footprint.Format = DXGI_FORMAT_R8_UINT;
	footprint.Footprint.Width = width;
	footprint.Footprint.Height = height;
	footprint.Footprint.Depth = 1;
	footprint.Footprint.RowPitch = (width + 255) & ~255; // D3D12_TEXTURE_DATA_PITCH_ALIGNMENT = 256

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.pResource = _shadingRateImage.Get();
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dstLocation.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = _vrsUploadBuffer.Get();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint = footprint;

	// 4. »óÅÂ ÀüÀÌ: COMMON -> COPY_DEST
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_shadingRateImage.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	GRAPHICS_CMD_LIST->ResourceBarrier(1, &barrier);

	// 5. º¹»ç
	GRAPHICS_CMD_LIST->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

	// 6. »óÅÂ ÀüÀÌ: COPY_DEST -> SHADING_RATE_SOURCE
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
	if (_supportsVRS && _shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2)
	{
		// º¸Åë ¿©±â¿¡ GEngine->GetWindowSize() °°Àº °É·Î È­¸é Å©±â ¹Þ¾Æ¿Ã ¼ö ÀÖ¾î¾ß ÇØ
		UINT width = static_cast<float>(GEngine->GetWindow().width);
		UINT height = static_cast<float>(GEngine->GetWindow().height);
		CreateVRSImage(width, height);
		CreateVRSUploadBuffer(width, height);
		UploadVRSData();  // ¼ÎÀÌµù ·¹ÀÌÆ® ¸Ê ÃÊ±âÈ­
	}
}

void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	// Á¤Á¡ ¹× ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	if (_shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_1) {
		_useTier2 = false;
	}
	else {
		_useTier2 = true;
	}
	
	/////////////////////////////// VRS /////////////////////////////////////
	ComPtr<ID3D12GraphicsCommandList5> commandList5;
	GRAPHICS_CMD_LIST->QueryInterface(IID_PPV_ARGS(&commandList5));

	if (commandList5 && _supportsVRS)
	{
		if (_useTier2 && _shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2 && _shadingRateImage)
		{
			// Tier 2 »ç¿ë
			commandList5->RSSetShadingRateImage(_shadingRateImage.Get());
			commandList5->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr); // ¾ÈÀü ÃÊ±âÈ­
		}
		else if (!_useTier2 && _shadingRateTier >= D3D12_VARIABLE_SHADING_RATE_TIER_1)
		{
			// Tier 1 »ç¿ë
			commandList5->RSSetShadingRateImage(nullptr);
			D3D12_SHADING_RATE_COMBINER combiners[] = {
				D3D12_SHADING_RATE_COMBINER_PASSTHROUGH,
				D3D12_SHADING_RATE_COMBINER_PASSTHROUGH
			};
			commandList5->RSSetShadingRate(shading_rate_size, combiners);
		}
	}

	// µå·Î¿ì È£Ãâ
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);

	// VRS ÇØÁ¦ (Tier 2 Àü¿ë)
	if (_useTier2 && _shadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2 && _shadingRateImage && commandList5)
	{
		commandList5->RSSetShadingRateImage(nullptr);
	}
	/////////////////////////////// VRS /////////////////////////////////////
	
	// µÎ ¹øÂ° µå·Î¿ì È£Ãâ (VRS ÇØÁ¦µÈ »óÅÂ)
	//GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);
	
}

void Mesh::Render(shared_ptr<InstancingBuffer>& buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

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
	// 1. ¹öÆÛ ºñ¾î ÀÖÀ½ ¹æ¾î
	if (buffer.empty())
	{
		wcout << L"[error] CreateVertexBuffer: Input vertexBuffer Empty" << endl;
		return;
	}

	// 2. ¹öÆÛ Å©±â °è»ê
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// 3. GPU ÀÚ¿ø »ý¼º
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

	// 4. µ¥ÀÌÅÍ º¹»ç
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

	// 5. View ÃÊ±âÈ­
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
				// FBX?ì„œ ?Œì‹±???•ë³´?¤ë¡œ ì±„ì›Œì¤€??
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
		// BoneOffet ?‰ë ¬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		_offsetBuffer = make_shared<StructuredBuffer>();
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
				for (int32 f = 0; f < keyFrameCount; f++)
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

			// StructuredBuffer ?¸íŒ…
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
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