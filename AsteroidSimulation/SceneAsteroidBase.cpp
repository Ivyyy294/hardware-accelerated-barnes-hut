#include "pch.h"

#include <IvyyyDirectionalLight.h>
#include <IvyyyCamera.h>
#include <IvyyyEngine.h>
#include <random>

#include "SceneAsteroidBase.h"
#include "DebugInfoUi.h"
#include "CameraMovement.h"
#include "Planet.h"
#include "SessionDurationController.h"
#include "Session.h"

SceneAsteroidBase::SceneAsteroidBase()
{
	m_sVar = Session::Me().m_variables;
}

void SceneAsteroidBase::Init()
{
	IvyyyPhysic::SetGravitationalConstant(m_sVar.ugc);

	//Add Light
	auto light = AddGameObject<GameObject>()->AddComponent<DirectionalLight>();
	light->GetTransform()->SetParent(Camera::MainCamera()->GetTransform());
	light->SetColor(Color(1.f, 1.f, 1.f, 1.0f));

	//Add Camera movement
	GameObject* camera = Camera::MainCamera()->GetGameObject();
	camera->AddComponent<CameraMovement>();

	//Add duration controller
	SessionDurationController* sdc = camera->AddComponent<SessionDurationController>().get();
	sdc->m_maxDurationSec = m_sVar.maxDurationSec;
	sdc->m_maxFixedFrames = m_sVar.maxFixedFrames;

	//Add Debug UI
	AddGameObject<DebugInfoUi>();

	//Add Planet
	Planet* planet = AddGameObject <Planet>();
	planet->GetComponent<PhysicObject>()->SetMass(Session::Me().m_variables.centralMass);
	OnOrbitalBodySpawned (planet);

	//Add Asteroids
	SpawnAsteroids (m_sVar.numAsteroids, m_sVar.distRange, m_sVar.eRange, m_sVar.iRange, planet);

	Session::Me().m_variables.Log();
}

void SceneAsteroidBase::SpawnAsteroids(int num, Vector2 dist, Vector2 e, Vector2 i, GameObject* planet)
{
		PhysicObject* soi = planet->GetComponent<PhysicObject>().get();

		dist = dist * 10.f;
		e = e * 10.f;

		SessionVariables variables = Session::Me().m_variables;

		std::string str (Session::Me().m_variables.seed);
		std::seed_seq seed1(str.begin(), str.end());
		std::mt19937 gen(seed1); // seed the generator
		std::normal_distribution<float> smaRnd(dist.x, dist.y); // define the range
		std::uniform_int_distribution<> eRnd(e.x, e.y); // define the range
		std::normal_distribution<float> massRnd(variables.massRangeAsteroids.x, variables.massRangeAsteroids.y);
		std::normal_distribution<float> iRnd(variables.iRange.x, variables.iRange.y);
		std::uniform_int_distribution<> oRnd(0, 360); // define the range
		std::uniform_int_distribution<> wRnd(0, 360); // define the range
		std::uniform_int_distribution<> m0Rnd(0, 360); // define the range

		for (int j = 0; j < num; ++j)
		{
			OrbitalElement oe;
			oe.a = smaRnd(gen) * 0.1f;
			oe.e = eRnd(gen) * 0.1f;
			oe.o = float (oRnd(gen));
			oe.w = float (wRnd(gen));
			oe.m0 = float (m0Rnd(gen));

			float mass = float(massRnd(gen));
			mass = max(mass, variables.minMassAsteroids);
			float size = mass / variables.massRangeAsteroids.x;

			Asteroid* asteroid = AddGameObject <Asteroid>();
			asteroid->SetPhysicalProperties(mass, size);
			asteroid->SetOrbit (oe, soi);

			OnOrbitalBodySpawned(asteroid);
		}
}

void SceneAsteroidBase::AdjustFixedUpdateLoop(ForceGeneratorBase* forceGenerator)
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[4];
	auto physicNode = mainLoopNodes->GetChildren()[0];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2, std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<ForceGeneratorBase, &ForceGeneratorBase::PreCalculateForces>(forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}