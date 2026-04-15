#include "pch.h"


#include <IvyyyPhysic.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "SceneDirectIntegration.h"

void SceneDirectIntegration::Init()
{
	SceneAsteroidBase::Init();
}

void SceneDirectIntegration::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	orbitalBody->AddComponent <PhysicObjectGravityGenerator>();
}
