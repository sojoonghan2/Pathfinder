#include "pch.h"
#include "OccupationScript.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Timer.h"
#include "Transform.h"

void OccupationScript::LateUpdate()
{
	auto pos = GetTransform()->GetLocalPosition();
	pos.y += 2.f;
	if (pos.y > 201.f) pos.y = -99.f;
	GetTransform()->SetLocalPosition(pos);

    m_accumulatedTime += DELTA_TIME * 1.5;
    GetGameObject()->GetMeshRenderer()->GetMaterial()->SetFloat(0, m_accumulatedTime);
}