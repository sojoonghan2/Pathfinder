#pragma once
#include "Object.h"

class Shader;
class Texture;

enum
{
	MATERIAL_ARG_COUNT = 4,
};

// 머터리얼 파라미터 구조
struct MaterialParams
{
	MaterialParams()
	{
		for (int32 i = 0; i < MATERIAL_ARG_COUNT; i++)
		{
			intParams[i] = 0;
			floatParams[i] = 0.f;
			texOnParams[i] = 0;
		}
	}

	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void SetFloat(uint8 index, float value) { floatParams[index] = value; }
	void SetTexOn(uint8 index, int32 value) { texOnParams[index] = value; }
	void SetVec2(uint8 index, Vec2 value) { vec2Params[index] = value; }
	void SetVec4(uint8 index, Vec4 value) { vec4Params[index] = value; }
	void SetMatrix(uint8 index, const Matrix& value) { matrixParams[index] = value; }

	array<int32, MATERIAL_ARG_COUNT> intParams;
	array<float, MATERIAL_ARG_COUNT> floatParams;
	array<int32, MATERIAL_ARG_COUNT> texOnParams;
	array<Vec2, MATERIAL_ARG_COUNT> vec2Params;
	array<Vec4, MATERIAL_ARG_COUNT> vec4Params;
	array<Matrix, MATERIAL_ARG_COUNT> matrixParams;
};

// 머터리얼 클래스
class Material : public Object
{
public:
	Material();
	virtual ~Material();

	// 클론
	shared_ptr<Material> Clone();

	// 셰이더
	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	shared_ptr<Shader> GetShader() const { return _shader; }

	// 파라미터 설정
	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.SetFloat(index, value); }
	void SetVec2(uint8 index, Vec2 value) { _params.SetVec2(index, value); }
	void SetVec4(uint8 index, Vec4 value) { _params.SetVec4(index, value); }
	void SetMatrix(uint8 index, const Matrix& mat) { _params.SetMatrix(index, mat); }

	void SetTexture(uint8 index, shared_ptr<Texture> texture);

	// 업로드
	void PushGraphicsData();
	void PushComputeData();

	void Dispatch(uint32 x, uint32 y, uint32 z);

private:
	void UploadCBV(bool isCompute);
	void BindTextures(bool isCompute);
	void BindShader();

private:
	shared_ptr<Shader> _shader;
	MaterialParams _params;
	array<shared_ptr<Texture>, MATERIAL_ARG_COUNT> _textures;
};