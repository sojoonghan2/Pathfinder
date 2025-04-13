#include "pch.h"
#include "BossScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

BossScript::BossScript() {}

BossScript::~BossScript() {}

void BossScript::LateUpdate() {
	// ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::ADD)) {
			ApplyModuleAbillities();
			GET_SINGLE(SceneManager)->LoadScene(L"TitleScene");
		}
	}
}