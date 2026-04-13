#include "pch.h"
#include "BarnesHutScene.h"

void BarnesHutScene::Init()
{
	m_forceGenerator = AddGameObject()->AddComponent<BarnesHutForceGenerator>().get();
	BaseAsteroidScene::Init();
}

void BarnesHutScene::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();
	
	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource (po);
}
