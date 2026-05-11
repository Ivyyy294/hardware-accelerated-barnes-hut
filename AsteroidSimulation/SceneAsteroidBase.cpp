#include "pch.h"

#include <IvyyyDirectionalLight.h>
#include <IvyyyCamera.h>
#include <IvyyyEngine.h>
#include <random>

#include "SceneAsteroidBase.h"
#include "DebugInfoUi.h"
#include "CameraMovement.h"
#include "Planet.h"
#include "SessionLoggerManager.h"
#include "FrameTimeRecorder.h"

SceneAsteroidBase::SceneAsteroidBase()
{
	m_sVar.numAsteroids = NUM_ASTEROIDS;
	m_sVar.ugc = 0.00005f;
	m_sVar.fixedDeltaTime = Time::UnscaledFixedDeltaTime();
	m_sVar.distRange = { 10.f, 15.f };
	m_sVar.eRange = { 0.f, 0.25f };
	m_sVar.iRange = { -10.f, 10.f };

}

void SceneAsteroidBase::Init()
{
	IvyyyPhysic::SetGravitationalConstant(m_sVar.ugc);

	//Add Light
	auto light = AddGameObject<GameObject>()->AddComponent<DirectionalLight>();
	light->GetTransform()->SetParent(Camera::MainCamera()->GetTransform());
	light->SetColor(Color(1.f, 1.f, 1.f, 1.0f));

	//Add Camera movement
	Camera::MainCamera()->GetGameObject()->AddComponent<CameraMovement>();
	Camera::MainCamera()->GetGameObject()->AddComponent<SessionLoggerManager>();
	Camera::MainCamera()->GetGameObject()->AddComponent<FrameTimeRecorder>();

	//Add Debug UI
	AddGameObject<DebugInfoUi>();

	//Add Planet
	Planet* planet = AddGameObject <Planet>();
	OnOrbitalBodySpawned (planet);

	//Add Asteroids
	SpawnAsteroids (m_sVar.numAsteroids, m_sVar.distRange, m_sVar.eRange, m_sVar.iRange, planet);

	LogSessionVariables();
	AdjustMainLoop();
}

void SceneAsteroidBase::SpawnAsteroids(int num, float dist, float speed)
{
	float angle = 360.f / ((float)num);
	Quaternion rotationOffset = Quaternion::Euler(0.f, 0.f, angle);
	Vector3 pos = Vector3::Up * dist;
	Vector3 vel = Vector3::Left * speed;

	for (int i = 0; i < num; ++i)
	{
		Asteroid* asteroid = AddGameObject <Asteroid>();
		asteroid->transform.SetPosition(pos);

		PhysicObject* rb = asteroid->GetComponent<PhysicObject>().get();
		rb->SetVelocity(vel);
		pos = rotationOffset * pos;
		vel = rotationOffset * vel;

		OnOrbitalBodySpawned (asteroid);
	}
}

void SceneAsteroidBase::SpawnAsteroids(int num, Vector2 dist, Vector2 e, Vector2 i, GameObject* planet)
{
		PhysicObject* soi = planet->GetComponent<PhysicObject>().get();

		dist = dist * 10.f;
		e = e * 10.f;

		std::string str ("85245679");
		std::seed_seq seed1(str.begin(), str.end());
		std::mt19937 gen(seed1); // seed the generator
		std::uniform_int_distribution<> smaRnd(dist.x, dist.y); // define the range
		std::uniform_int_distribution<> eRnd(e.x, e.y); // define the range
		std::uniform_int_distribution<> iRnd(i.x, i.y); // define the range
		std::uniform_int_distribution<> oRnd(0, 360); // define the range
		std::uniform_int_distribution<> wRnd(0, 360); // define the range
		std::uniform_int_distribution<> m0Rnd(0, 360); // define the range

		for (int j = 0; j < num; ++j)
		{
			OrbitalElement oe;
			oe.a = smaRnd(gen) * 0.1f;
			oe.e = eRnd(gen) * 0.1f;
			oe.i = float (iRnd(gen));
			oe.o = float (oRnd(gen));
			oe.w = float (wRnd(gen));
			oe.m0 = float (m0Rnd(gen));

			Asteroid* asteroid = AddGameObject <Asteroid>();
			asteroid->SetOrbit (oe, soi);

			OnOrbitalBodySpawned(asteroid);
		}
}

void SceneAsteroidBase::LogSessionVariables()
{
	SessionLoggerManager* slm = SessionLoggerManager::Me();
	slm->m_variables.WriteLine("method;" + m_sVar.method);
	slm->m_variables.WriteLine("numAsteroids;" + std::to_string(m_sVar.numAsteroids));
	slm->m_variables.WriteLine("ugc;" + std::to_string(m_sVar.ugc));
	slm->m_variables.WriteLine("fixedDeltaTime;" + std::to_string(m_sVar.fixedDeltaTime));
	slm->m_variables.WriteLine("distRange;" + std::to_string(m_sVar.distRange.x) + ";" + std::to_string(m_sVar.distRange.y));
	slm->m_variables.WriteLine("eRange;" + std::to_string(m_sVar.eRange.x) + ";" + std::to_string(m_sVar.eRange.y));
	slm->m_variables.WriteLine("iRange;" + std::to_string(m_sVar.iRange.x) + ";" + std::to_string(m_sVar.iRange.y));
}

void SceneAsteroidBase::AdjustMainLoop()
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode& node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren();

	FrameTimeRecorder* recorder = FrameTimeRecorder::Me();
	mainLoopNodes.emplace(mainLoopNodes.begin() + 2, std::make_shared <MainLoopNode>("LogFrameTime", Event().Bind<FrameTimeRecorder, &FrameTimeRecorder::LogFrameTime>(recorder)));
	node.SetChildren(mainLoopNodes);
}

void SceneAsteroidBase::AdjustFixedUpdateLoop(ForceGeneratorBarnesHutBase* forceGenerator)
{
	MainLoop& mainLoop = IvyyyEngine::Me()->GetMainLoop();
	MainLoopNode node = mainLoop.GetNode();
	auto mainLoopNodes = node.GetChildren()[5];
	auto physicNode = mainLoopNodes->GetChildren()[0];

	auto fixedUpdateNodes = physicNode->GetChildren();
	fixedUpdateNodes.emplace(fixedUpdateNodes.begin() + 2, std::make_shared <MainLoopNode>("BarnesHutCSForceGenerator", Event().Bind<ForceGeneratorBarnesHutBase, &ForceGeneratorBarnesHutBase::PreCalculateForces>(forceGenerator)));
	physicNode->SetChildren(fixedUpdateNodes);
}