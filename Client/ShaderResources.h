#pragma once
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

// 셰이더로 생성하는 리소스
class ShaderResources
{
	DECLARE_SINGLE(ShaderResources);

public:
	void Init();

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();
};