#include "pch.h"
#include "LuckyScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

LuckyScript::LuckyScript() {}

LuckyScript::~LuckyScript() {}

void LuckyScript::LateUpdate() {

    // 게임 윈도우가 포커스 되었을 때만 입력 처리
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {  // 0은 왼쪽 마우스 버튼
            GET_SINGLE(SceneManager)->LoadScene(L"ParticleScene");
        }
    }
}