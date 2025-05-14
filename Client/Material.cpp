#include "pch.h"
#include "Material.h"
#include "Application.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL)
{
}

Material::~Material()
{
}

void Material::PushGraphicsData()
{
	// CBV ���ε�
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));

	// SRV ���ε�
	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		P_Application->GetGraphicsDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// ���������� ����
	_shader->Update();
}

void Material::PushComputeData()
{
	// CBV ���ε�
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));

	// SRV ���ε�
	for (size_t i = 0; i < min(_textures.size(), 7); i++)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		P_Application->GetComputeDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// ���������� ����
	_shader->Update();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	// CBV + SRV + SetPipelineState
	PushComputeData();

	// SetDescriptorHeaps + SetComputeRootDescriptorTable
	P_Application->GetComputeDescHeap()->CommitTable();

	COMPUTE_CMD_LIST->Dispatch(x, y, z);

	P_Application->GetComputeCmdQueue()->UploadComputeResource();
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();

	material->SetShader(_shader);
	material->_params = _params;
	material->_textures = _textures;

	return material;
}