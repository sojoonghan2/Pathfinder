#include "pch.h"
#include "FactoryScript.h"
#include "Input.h"
#include "Application.h"
#include "SceneManager.h"
#include "GameObject.h"

FactoryScript::FactoryScript() {}

FactoryScript::~FactoryScript() {}

void FactoryScript::Start()
{
	INPUT->LockCursor(true);
}

void FactoryScript::LateUpdate()
{
}