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
    if (pos.y > 900.f) pos.y = 0.f;
    GetTransform()->SetLocalPosition(pos);
}
