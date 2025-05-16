#include "pch.h"
#include "Material.h"
#include "GameFramework.h"
#include "Shader.h"
#include "Texture.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL) {}
Material::~Material() {}

void Material::SetTexture(uint8 index, shared_ptr<Texture> texture)
{
	_textures[index] = texture;
	_params.SetTexOn(index, (texture == nullptr ? 0 : 1));
}

void Material::PushGraphicsData()
{
	UploadCBV(false);
	BindTextures(false);
	BindShader();
}

void Material::PushComputeData()
{
	UploadCBV(true);
	BindTextures(true);
	BindShader();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	PushComputeData();
	GFramework->GetComputeDescHeap()->CommitTable();
	COMPUTE_CMD_LIST->Dispatch(x, y, z);
	GFramework->GetComputeCmdQueue()->UploadComputeResource();
}

void Material::UploadCBV(bool isCompute)
{
	if (isCompute)
		CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));
	else
		CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));
}

void Material::BindTextures(bool isCompute)
{
	const uint8 maxCount = static_cast<uint8>(_textures.size());
	for (uint8 i = 0; i < maxCount; ++i)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		if (isCompute)
			GFramework->GetComputeDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
		else
			GFramework->GetGraphicsDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}
}

void Material::BindShader()
{
	if (_shader)
		_shader->Update();
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(_shader);
	material->_params = _params;
	material->_textures = _textures;
	return material;
}