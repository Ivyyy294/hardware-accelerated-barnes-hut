#include "pch.h"


#include <IvyyyPhysic.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "DirectIntegrationScene.h"

void DirectIntegrationScene::Init()
{
	BaseAsteroidScene::Init();
}

void DirectIntegrationScene::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	orbitalBody->AddComponent <PhysicObjectGravityGenerator>();
}
