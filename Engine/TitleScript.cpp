#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

TitleScript::TitleScript() {}

TitleScript::~TitleScript() {}

void TitleScript::LateUpdate() {

    // 게임 윈도우가 포커스 되었을 때만 입력 처리
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::ADD)) {
            GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
        }
    }
}