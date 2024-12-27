#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

unique_ptr<Engine> GEngine = make_unique<Engine>();

wstring s2ws(const string& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ws2s(const wstring& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

Vec3 Normalization(Vec3 vec) {
	float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	// ������ ���̰� 0�� ��� ����ȭ �� �� �����Ƿ� üũ
	if (length == 0) {
		return Vec3(0, 0, 0); // ���̰� 0�� ��� 0���͸� ��ȯ
	}
	return Vec3(vec.x / length, vec.y / length, vec.z / length);
}

bool IsMatrixValid(const XMFLOAT4X4& matrix)
{
    // XMFLOAT4X4�� ��� ��Ұ� 0�� �ƴ� ��� ��ȿ�ϴٰ� ����
    return matrix._11 != 0 || matrix._12 != 0 || matrix._13 != 0 || matrix._14 != 0 ||
        matrix._21 != 0 || matrix._22 != 0 || matrix._23 != 0 || matrix._24 != 0 ||
        matrix._31 != 0 || matrix._32 != 0 || matrix._33 != 0 || matrix._34 != 0 ||
        matrix._41 != 0 || matrix._42 != 0 || matrix._43 != 0 || matrix._44 != 0;
}

void DecomposeMatrix(const XMFLOAT4X4& matrix, Vec3& position, Quaternion& rotation, Vec3& scale)
{
    // 4x4 ����� XMMATRIX�� ��ȯ
    XMMATRIX mat = XMLoadFloat4x4(&matrix);

    // ��ġ, ȸ��, ������ ���͸� �ʱ�ȭ
    XMVECTOR posVec, rotQuat, sclVec;

    // ����� ��ġ, ȸ��(Quaternion), �����Ϸ� �и�
    XMMatrixDecompose(&sclVec, &rotQuat, &posVec, mat);

    // �и��� ������ SimpleMath�� Vector3�� Quaternion �������� ��ȯ
    position = Vector3(posVec);
    rotation = Quaternion(rotQuat);
    scale = Vector3(sclVec);
}

Vec3 QuaternionToEuler(const Quaternion& q)
{
    // ���ʹϾ��� ��ķ� ��ȯ
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(q);

    // ����� ��� ����
    float pitch, yaw, roll;

    // ����� ��Ҹ� ����Ͽ� ���Ϸ� �� ���
    // Pitch (X-axis rotation)
    pitch = std::asin(-rotationMatrix.r[2].m128_f32[1]);

    // Roll (Z-axis rotation)
    if (std::cos(pitch) > 0.0001f) { // �������� ���� ���� �� ��
        roll = std::atan2(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
        yaw = std::atan2(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);
    }
    else {
        roll = std::atan2(-rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
        yaw = 0.0f;
    }

    // ���Ϸ� ���� Vector3�� ��ȯ (Yaw, Pitch, Roll)
    return Vector3(pitch, yaw, roll); // ��ġ, ��, �� ������ ��ȯ
}