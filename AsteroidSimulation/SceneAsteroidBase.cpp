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
	sdc->m_maxFixedFrames = m_sVar.maxFixedFrames;

	//Add Debug UI
	AddGameObject<DebugInfoUi>();

	//Add Sun
	Planet* sun = AddGameObject <Planet>();
	sun->SetData (Color(252, 120, 1), Session::Me().m_variables.centralMass, 2.0f);
	OnOrbitalBodySpawned (sun);

	//Add Asteroids
	SpawnAsteroids (m_sVar.numAsteroids, sun);

	Session::Me().m_variables.Log();
}

void SceneAsteroidBase::SpawnAsteroids(int num, GameObject* planet)
{
		PhysicObject* soi = planet->GetComponent<PhysicObject>().get();

		SessionVariables variables = Session::Me().m_variables;

		Vector2 dist = variables.distRange * 10.f;
		Vector2 e = variables.eRange * 10.f;


		const float minVal = 0.001f;

		std::string str (Session::Me().m_variables.seed);
		std::seed_seq seed1(str.begin(), str.end());
		std::mt19937 gen(seed1); // seed the generator
		std::normal_distribution<float> smaRnd(dist.x, max(minVal, dist.y)); // define the range
		std::uniform_int_distribution<> eRnd(e.x, e.y); // define the range
		std::normal_distribution<float> massRnd(variables.massRangeAsteroids.x, max(minVal, variables.massRangeAsteroids.y));
		std::normal_distribution<float> iRnd(variables.iRange.x, max(minVal, variables.iRange.y));
		std::uniform_int_distribution<> oRnd(0, 360); // define the range
		std::uniform_int_distribution<> wRnd(0, 360); // define the range
		std::uniform_int_distribution<> m0Rnd(0, 360); // define the range

		for (int j = 0; j < num; ++j)
		{
			OrbitalElement oe;

			oe.a = dist.y >= minVal ? smaRnd(gen) * 0.1f : dist.x * 0.1f;
			oe.e = eRnd(gen) * 0.1f;
			oe.i = variables.iRange.y >= minVal ? iRnd(gen) : variables.iRange.x;
			oe.o = float (oRnd(gen));
			oe.w = float (wRnd(gen));
			oe.m0 = float (m0Rnd(gen));

			float mass = variables.massRangeAsteroids.y >= minVal ? massRnd(gen) : variables.massRangeAsteroids.x;
			mass = max(mass, variables.minMassAsteroids);
			float size = mass / variables.massRangeAsteroids.x;

			Asteroid* asteroid = AddGameObject <Asteroid>();
			asteroid->SetPhysicalProperties(mass, 0.05f * size);
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