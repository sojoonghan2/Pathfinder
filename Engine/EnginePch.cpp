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

float MyProject::Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float MyProject::Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

Quaternion MyProject::Normalize(const Quaternion& q)
{
	float magnitude = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	if (magnitude > 1e-6f) {
		return Quaternion(q.x / magnitude, q.y / magnitude, q.z / magnitude, q.w / magnitude);
	}
	else {
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); // �⺻ ���� ���ʹϾ� ��ȯ
	}
}

Vec3 MyProject::Lerp(const Vec3& start, const Vec3& end, float t)
{
	return start * (1.0f - t) + end * t;
}

Quaternion MyProject::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float dotProduct = Dot(q1, q2);

	// ������ �ݴ��� ������ �������� ����
	Quaternion q2Adjusted = dotProduct < 0.0f ? -q2 : q2;

	Quaternion result = q1 * (1.0f - t) + q2Adjusted * t;

	// ��� ���ʹϾ��� ����ȭ
	return Normalize(result);
}

Quaternion MyProject::QuaternionSlerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cosTheta = Dot(q1, q2);
	Quaternion q2Adjusted = cosTheta < 0.0f ? -q2 : q2;

	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	if (sinTheta < 1e-6f) {
		return MyProject::Lerp(q1, q2Adjusted, t);
	}
	else {
		float angle = atan2(sinTheta, cosTheta);
		float w1 = sin((1.0f - t) * angle) / sinTheta;
		float w2 = sin(t * angle) / sinTheta;
		return q1 * w1 + q2Adjusted * w2;
	}
}

Matrix MyProject::MatrixAffineTransformation(const Vec3& scale, const Quaternion& rotation, const Vec3& translation)
{
	Matrix result;
	result = MatrixScaling(scale) * MatrixRotationQuaternion(rotation);
	result._41 = translation.x;
	result._42 = translation.y;
	result._43 = translation.z;
	return result;
}

Matrix MyProject::MatrixScaling(const Vec3& scale)
{
	Matrix result = Matrix::Identity;

	result._11 = scale.x;
	result._22 = scale.y;
	result._33 = scale.z;

	return result;
}

Matrix MyProject::MatrixRotationQuaternion(const Quaternion& quat)
{
	Matrix result = Matrix::Identity;

	float xx = quat.x * quat.x;
	float yy = quat.y * quat.y;
	float zz = quat.z * quat.z;

	float xy = quat.x * quat.y;
	float xz = quat.x * quat.z;
	float yz = quat.y * quat.z;

	float wx = quat.w * quat.x;
	float wy = quat.w * quat.y;
	float wz = quat.w * quat.z;

	result._11 = 1.0f - 2.0f * (yy + zz);
	result._12 = 2.0f * (xy - wz);
	result._13 = 2.0f * (xz + wy);

	result._21 = 2.0f * (xy + wz);
	result._22 = 1.0f - 2.0f * (xx + zz);
	result._23 = 2.0f * (yz - wx);

	result._31 = 2.0f * (xz - wy);
	result._32 = 2.0f * (yz + wx);
	result._33 = 1.0f - 2.0f * (xx + yy);

	return result;
}

// ��(degree) -> ����(radian) ��ȯ �Լ�
float ToRadian(float degree)
{
	return degree * (PI / 180.0f);
}