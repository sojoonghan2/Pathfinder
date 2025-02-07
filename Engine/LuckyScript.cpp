#include "pch.h"
#include "LuckyScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

LuckyScript::LuckyScript() {}

LuckyScript::~LuckyScript() {}

void LuckyScript::LateUpdate() {

    // ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {  // 0�� ���� ���콺 ��ư
            GET_SINGLE(SceneManager)->LoadScene(L"ParticleScene");
        }
    }
}