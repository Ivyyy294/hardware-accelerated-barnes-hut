#include "pch.h"
#include "BarnesHutScene.h"
#include <IvyyyEngine.h>

void BarnesHutScene::Init()
{
	m_forceGenerator = AddGameObject()->AddComponent<BarnesHutForceGenerator>().get();
	BaseAsteroidScene::Init();

	AdjustFixedUpdateLoop();
}

void BarnesHutScene::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();
	
	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource (po);
}

void BarnesHutScene::AdjustFixedUpdateLoop()
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[3];
	auto physicNode = mainLoopNodes->GetChildren()[1];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2, std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<BarnesHutForceGenerator, &BarnesHutForceGenerator::BuildTree>(m_forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}