#pragma once
#include "MonoBehaviour.h"
#include "PlayerScript.h"

class BulletScript : public MonoBehaviour
{
public:
    BulletScript(shared_ptr<PlayerScript> playerScript);
    virtual ~BulletScript();

    virtual void Update() override;

private:
    void MouseInput();
    void MoveBullet();

private:
    shared_ptr<PlayerScript> _playerScript;
    shared_ptr<class Transform> _parentTransform;

    Vec3 _velocity = Vec3::Zero;
    bool _isFired = false;

    float _lifeTime = 0.f;

    // �Ѿ� �ε���
    static inline int32 s_currentBulletIndex = 0;
    // ���� ����
    static inline float s_lastFireTime = 0.f;
    // �Ѿ� �߻� ������
    static constexpr float s_fireInterval = 10.f;

    int32 _myIndex = 0;
    static inline int32 s_bulletCount = 0;
};