#include "pch.h"

#include <IvyyyDirectionalLight.h>
#include <IvyyyCamera.h>

#include "SceneAsteroidBase.h"
#include "DebugInfoUi.h"
#include "CameraMovement.h"
#include "Planet.h"

void SceneAsteroidBase::Init()
{
	//Add Light
	auto light = AddGameObject<GameObject>()->AddComponent<DirectionalLight>();
	light->GetTransform()->SetParent(Camera::MainCamera()->GetTransform());
	light->SetColor(Color(1.f, 1.f, 1.f, 1.0f));

	//Add Camera movement
	Camera::MainCamera()->GetGameObject()->AddComponent<CameraMovement>();

	//Add Debug UI
	AddGameObject<DebugInfoUi>();

	//Add Asteroids
	SpawnAsteroids(64, 10.f, 0.5f);
	SpawnAsteroids(128, 10.25f, 0.5f);
	//SpawnAsteroids(128, 10.5f, 0.5f);
	//SpawnAsteroids(256, 10.75f, 0.48f);
	//SpawnAsteroids(512, 16.f, 0.45f);
	//SpawnAsteroids(512, 16.25f, 0.45f);
	//SpawnAsteroids(512, 16.5f, 0.45f);
	//SpawnAsteroids(512, 16.75f, 0.45f);

	//Add Planet
	Planet* planet = AddGameObject <Planet>();
	OnOrbitalBodySpawned (planet);

	IvyyyPhysic::SetGravitationalConstant(0.005f);
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
