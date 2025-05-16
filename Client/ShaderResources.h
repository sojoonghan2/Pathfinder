#pragma once
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class ShaderResources
{
	DECLARE_SINGLE(ShaderResources);

public:
	void Init();

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();
};