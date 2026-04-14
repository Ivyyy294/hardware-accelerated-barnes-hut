#include "pch.h"
#include "BarnesHutCSScene.h"

void BarnesHutCSScene::Init()
{
	m_forceGenerator = AddGameObject()->AddComponent<BarnesHutCSForceGenerator>().get();
	BaseAsteroidScene::Init();
}

void BarnesHutCSScene::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
}
