#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

TitleScript::TitleScript() {}

TitleScript::~TitleScript() {}

void TitleScript::LateUpdate() {

    // ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::LBUTTON)) {  // 0�� ���� ���콺 ��ư
            GET_SINGLE(SceneManager)->LoadScene(L"LuckyScene");
        }
    }
}