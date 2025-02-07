#include "pch.h"
#include "ExplorationScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

ExplorationScript::ExplorationScript() {}

ExplorationScript::~ExplorationScript() {}

void ExplorationScript::LateUpdate() {

    // ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
            ApplyModuleAbillities();
            GET_SINGLE(SceneManager)->LoadScene(L"CrashScene");
        }
    }
}