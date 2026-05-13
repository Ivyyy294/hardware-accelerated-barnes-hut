#include "pch.h"


#include <IvyyyPhysic.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "SceneDirectIntegration.h"

void SceneDirectIntegration::Init()
{
	m_sVar.method = SessionVariables::DIRECT;
	m_forceGenerator = AddGameObject()->AddComponent<ForceGeneratorDirect>().get();
	SceneAsteroidBase::Init();
	AdjustFixedUpdateLoop(m_forceGenerator);
}

void SceneDirectIntegration::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();

	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource(po);
}
