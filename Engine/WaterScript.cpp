#include "pch.h"
#include "WaterScript.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Timer.h"

void WaterScript::LateUpdate()
{
	if (GetMeshRenderer())
		GetMeshRenderer()->GetMaterial()->SetFloat(0, GET_SINGLE(Timer)->GetTotalTime() * 3.f);
}
