#pragma once


#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")


// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0

// 각종 include
#include <windows.h>
#include <tchar.h>

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <queue>

#include <fstream>
#include <print>

#include <random>

#include <thread>
#include <chrono>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;
#include "../Common/protocol.h"
#include "../Common/util.h"

#include "d3dx12.h"
#include "SimpleMath.h"
#include "SimpleMath.inl"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#include "FBX/fbxsdk.h"

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif

// 콘솔 창 출력
#include <iostream>
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define WINDOWHEIGHT	1280
#define WINDOWWIDTH		800

#define SCENESIZE		1000.f
#define MAXMOVESIZE		500.f

#define PI 3.14159265358979323846f

// 각종 typedef
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using Vec2		= DirectX::SimpleMath::Vector2;
using Vec3		= DirectX::SimpleMath::Vector3;
using Vec4		= DirectX::SimpleMath::Vector4;
using Matrix	= DirectX::SimpleMath::Matrix;

enum class CBV_REGISTER : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class SRV_REGISTER : uint8
{
	t0 = static_cast<uint8>(CBV_REGISTER::END),
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,

	END
};

enum class UAV_REGISTER : uint8
{
	u0 = static_cast<uint8>(SRV_REGISTER::END),
	u1,
	u2,
	u3,
	u4,

	END,
};

enum class SAMPLER_REGISTER : uint8
{
	s0 = static_cast<uint8>(UAV_REGISTER::END),
	s1,
	s2,
	s3,
	s4,

	END
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,
	CBV_SRV_REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,
	UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
	TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT
};

struct WindowInfo
{
	HWND	hwnd; // 출력 윈도우
	int32	width; // 너비
	int32	height; // 높이
	bool	windowed; // 창모드 or 전체화면
};

struct Vertex
{
	Vertex() {}

	Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
		: pos(p), uv(u), normal(n), tangent(t)
	{
	}

	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec4 weights;
	Vec4 indices;
};

// 싱글톤 매크로
#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define COMPUTE_CMD_LIST	GEngine->GetComputeCmdQueue()->GetComputeCmdList()

#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define INPUT				GET_SINGLE(Input)
#define DELTA_TIME			GET_SINGLE(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

#define PRINTPOSITION \
    std::cout << std::string(GetGameObject()->GetName().begin(), GetGameObject()->GetName().end()) << ": " << "(" << GetTransform()->GetLocalPosition().x << ", " \
              << GetTransform()->GetLocalPosition().y << ", " \
              << GetTransform()->GetLocalPosition().z << ")" << std::endl


struct TransformParams
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProjection;
	Matrix matWV;
	Matrix matWVP;
	Matrix matViewInv;
};

struct AnimFrameParams
{
	Vec4	scale;
	Vec4	rotation; // Quaternion
	Vec4	translation;
};

enum ModuleType
{
	ATKUp, // 공격력 증폭
	DEFUp, // 방어력 증폭
	HPUp, // 체력 증폭
	CRTDamageUp, // 치명타 데미지 증폭
	CRTRateUp, // 치명타 확률 증폭
	BattleFanatic, // 전투광
	Adjust, // 적응
	OverHeating, // 과열
	Gamester, // 도박꾼
	EmergencyAssistance, // 응급 지원
	Armor, // 철갑
	EnhancedSiege // 화력 강화
};

// 전역 인스턴스를 선언하여 엔진을 전역적으로 접근 가능하게 함
extern unique_ptr<class Engine> GEngine;

// 문자열 변환
wstring s2ws(const string& s);
string ws2s(const wstring& s);

// 정규화
Vec3 Normalization(Vec3 vec);

bool IsMatrixValid(const XMFLOAT4X4& matrix);
void DecomposeMatrix(const XMFLOAT4X4& matrix, Vec3& position, Quaternion& rotation, Vec3& scale);
Vec3 QuaternionToEuler(const Quaternion& q);
float ToRadian(float degree);

namespace MyProject
{
	float Dot(const Vector3& v1, const Vector3& v2);

	float Dot(const Quaternion& q1, const Quaternion& q2);

	Quaternion Normalize(const Quaternion& q);

	Vec3 Lerp(const Vec3& start, const Vec3& end, float t);

	Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

	Quaternion QuaternionSlerp(const Quaternion& q1, const Quaternion& q2, float t);

	Matrix MatrixAffineTransformation(const Vec3& scale, const Quaternion& rotation, const Vec3& translation);

	Matrix MatrixScaling(const Vec3& scale);

	Matrix MatrixRotationQuaternion(const Quaternion& quat);
}
