#include "pch.h"


#include <IvyyyPhysic.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "SceneDirectIntegration.h"

void SceneDirectIntegration::Init()
{
	m_sVar.method = "direct";
	SceneAsteroidBase::Init();
}

void SceneDirectIntegration::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	orbitalBody->AddComponent <PhysicObjectGravityGenerator>();
}
