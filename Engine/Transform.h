#pragma once
#include "Component.h"

class Transform : public Component
{
public:
    Transform();
    virtual ~Transform();

    virtual void FinalUpdate() override;
    void PushData();

public:
    // Parent 기준
    const Vec3& GetLocalPosition() { return _localPosition; }
    const Vec3& GetLocalRotation() { return _localRotation; }
    const Vec3& GetLocalRevolution() { return _localRevolution; }
    const Vec3& GetLocalScale() { return _localScale; }

    float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

    const Matrix& GetLocalToWorldMatrix() { return _matWorld; }
    Vec3 GetWorldPosition() { return _matWorld.Translation(); }

    const Quaternion GetLocalRotationQuat() { return _quaternion; }

    Vec3 GetRight() { return _matWorld.Right(); }
    Vec3 GetUp() { return _matWorld.Up(); }
    Vec3 GetLook() { return _matWorld.Backward(); }

    void SetLocalPosition(const Vec3& position) { _localPosition = position; }
    void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; }
    void SetLocalRevolution(const Vec3& rotation) { _localRevolution = rotation; }
    void SetLocalScale(const Vec3& scale) { _localScale = scale; }

    void LookAt(const Vec3& dir);
    void SetLocalRotation(const Quaternion& quaternion) { _quaternion = quaternion; }
    static bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
    static Vec3 DecomposeRotationMatrix(const Matrix& rotation);
    Vec3 QuaternionToEuler(const Quaternion& q);

public:
    // 부모 설정
    void SetParent(shared_ptr<Transform> parent)
    {
        _previousParent = _parent;
        _parent = parent;
    }

    // 부모 제거
    void RemoveParent()
    {
        _previousParent = _parent;
        _parent.reset();
    }

    // 부모 복구
    void RestoreParent()
    {
        if (!_previousParent.expired())
        {
            _parent = _previousParent.lock();
        }
    }

    weak_ptr<Transform> GetParent() { return _parent; }

private:
    // Parent 기준
    Vec3 _localPosition = {};
    Vec3 _localRotation = {};
    Vec3 _localRevolution = {};
    Vec3 _localScale = { 1.f, 1.f, 1.f };
    Quaternion _quaternion;

    Matrix _matLocal = {};
    Matrix _matWorld = {};

    weak_ptr<Transform> _parent;
    weak_ptr<Transform> _previousParent;
};
