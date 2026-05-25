#include "pch.h"
#include "SceneBarnesHutCS.h"
#include <IvyyyEngine.h>

void SceneBarnesHutCS::Init()
{
	m_sVar.method = SessionVariables::BARNES_HUT_GPU;
	m_forceGenerator = AddGameObject()->AddComponent<ForceGeneratorBarnesHutCS>().get();
	SceneAsteroidBase::Init();

	AdjustFixedUpdateLoop (m_forceGenerator);
}

void SceneBarnesHutCS::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();

	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource(po);
}
