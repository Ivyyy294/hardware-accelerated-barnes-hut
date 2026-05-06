#include "pch.h"
#include "SceneBarnesHutCS.h"
#include <IvyyyEngine.h>

void SceneBarnesHutCS::Init()
{
	m_sVar.method = "barnes-hut-cs";
	m_forceGenerator = AddGameObject()->AddComponent<ForceGeneratorBarnesHutCS>().get();
	SceneAsteroidBase::Init();

	AdjustFixedUpdateLoop();
}

void SceneBarnesHutCS::AdjustFixedUpdateLoop()
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[3];
	auto physicNode = mainLoopNodes->GetChildren()[1];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2,std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<ForceGeneratorBarnesHutCS, &ForceGeneratorBarnesHutCS::Dispatch>(m_forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}

void SceneBarnesHutCS::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();

	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource(po);
}
