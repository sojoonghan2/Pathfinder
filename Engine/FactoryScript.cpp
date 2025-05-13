#include "pch.h"
#include "FactoryScript.h"
#include "Input.h"
#include "Engine.h"
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