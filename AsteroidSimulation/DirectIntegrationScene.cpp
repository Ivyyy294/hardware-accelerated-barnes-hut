#include "pch.h"

#include <IvyyyDirectionalLight.h>
#include <IvyyyCamera.h>
#include <IvyyyPhysic.h>

#include "DirectIntegrationScene.h"
#include "Asteroid.h"
#include "Planet.h"
#include "DebugInfoUi.h"

void DirectIntegrationScene::Init()
{
	//Add Light
	auto light = AddGameObject<GameObject>()->AddComponent<DirectionalLight>();
	light->GetTransform()->SetParent(Camera::MainCamera()->GetTransform());
	light->SetColor(Color(1.f, 1.f, 1.f, 1.0f));

	//Add Debug UI
	AddGameObject<DebugInfoUi>();

	//Add Asteroids
	SpawnAsteroids(64, 4.f, 0.5f);
	SpawnAsteroids(128, 4.25f, 0.5f);
	SpawnAsteroids(128, 4.5f, 0.5f);
	SpawnAsteroids(256, 4.75f, 0.48f);
	SpawnAsteroids(256, 5.f, 0.45f);

	//Add Planet
	AddGameObject <Planet>();

	IvyyyPhysic::SetGravitationalConstant(0.01f);
}

void DirectIntegrationScene::SpawnAsteroids (int num, float dist, float speed)
{
	float angle = 360.f / ((float)num);
	Quaternion rotationOffset = Quaternion::Euler (0.f, 0.f, angle);
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
	}
}
