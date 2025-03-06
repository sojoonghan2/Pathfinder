#include "pch.h"
#include "RuinsScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

RuinsScript::RuinsScript() {}

RuinsScript::~RuinsScript() {}

void RuinsScript::LateUpdate() {

    // ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
            ApplyModuleAbillities();
            GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
        }
    }

    Occupation();
}

void RuinsScript::Occupation()
{
    if(GET_SINGLE(SceneManager)->FindObjectByName(L"Water")) _water = GET_SINGLE(SceneManager)->FindObjectByName(L"Water");
    if(GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ")) _player = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");

    auto playerPos = _player->GetTransform()->GetLocalPosition();

    if (playerPos.x >= -2000.0f && playerPos.x <= 2000.0f &&
        playerPos.z >= -2000.0f && playerPos.z <= 2000.0f)
    {
        Vec3 pos = _water->GetTransform()->GetLocalPosition();
        pos.y -= 0.2f;
        _water->GetTransform()->SetLocalPosition(pos);
    }
    else
    {
        Vec3 pos = _water->GetTransform()->GetLocalPosition();
        pos.y += 0.1f;
        _water->GetTransform()->SetLocalPosition(pos);
    }

}
