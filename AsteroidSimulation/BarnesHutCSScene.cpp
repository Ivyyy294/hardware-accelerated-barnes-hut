#include "pch.h"
#include "BarnesHutCSScene.h"
#include <IvyyyEngine.h>

void BarnesHutCSScene::Init()
{
	m_forceGenerator = AddGameObject()->AddComponent<BarnesHutCSForceGenerator>().get();
	BaseAsteroidScene::Init();

	AdjustFixedUpdateLoop();
}

void BarnesHutCSScene::AdjustFixedUpdateLoop()
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[3];
	auto physicNode = mainLoopNodes->GetChildren()[1];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2,std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<BarnesHutCSForceGenerator, &BarnesHutCSForceGenerator::Dispatch>(m_forceGenerator)));
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 3,std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<BarnesHutCSForceGenerator, &BarnesHutCSForceGenerator::ReadBack>(m_forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}

void BarnesHutCSScene::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
}
