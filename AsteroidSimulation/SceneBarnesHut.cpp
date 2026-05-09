#include "pch.h"
#include "SceneBarnesHut.h"
#include <IvyyyEngine.h>

void SceneBarnesHut::Init()
{
	//Gizmo::Draw = true;
	m_sVar.method = SessionVariables::BARNES_HUT_CPU;
	m_forceGenerator = AddGameObject()->AddComponent<ForceGeneratorBarnesHut>().get();
	SceneAsteroidBase::Init();

	AdjustFixedUpdateLoop();
}

void SceneBarnesHut::OnOrbitalBodySpawned(GameObject* orbitalBody)
{
	PhysicObject* po = orbitalBody->GetComponent<PhysicObject>().get();
	
	if (po == nullptr)
		return;

	m_forceGenerator->AddForceSource (po);
}

void SceneBarnesHut::AdjustFixedUpdateLoop()
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[3];
	auto physicNode = mainLoopNodes->GetChildren()[1];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2, std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<ForceGeneratorBarnesHut, &ForceGeneratorBarnesHut::BuildTree>(m_forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}