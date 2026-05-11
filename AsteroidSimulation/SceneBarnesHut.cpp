#include "pch.h"
#include "SceneBarnesHut.h"
#include <IvyyyEngine.h>

void SceneBarnesHut::Init()
{
	//Gizmo::Draw = true;
	m_sVar.method = SessionVariables::BARNES_HUT_CPU;
	m_forceGenerator = AddGameObject()->AddComponent<ForceGeneratorBarnesHut>().get();
	SceneAsteroidBase::Init();

	AdjustFixedUpdateLoop(m_forceGenerator);
}

void SceneBarnesHut::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();
	
	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource (po);
}
