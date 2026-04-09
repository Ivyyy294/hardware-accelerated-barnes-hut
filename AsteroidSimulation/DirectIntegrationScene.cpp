#include "pch.h"

#include <IvyyyDirectionalLight.h>
#include <IvyyyCamera.h>

#include "DirectIntegrationScene.h"
#include "Asteroid.h"


void DirectIntegrationScene::Init()
{
	//Add Light
	auto light = AddGameObject<GameObject>()->AddComponent<DirectionalLight>();
	light->GetTransform()->SetParent(Camera::MainCamera()->GetTransform());
	light->SetColor(Color(1.f, 1.f, 1.f, 1.0f));

	AddGameObject <Asteroid>()->transform.SetPosition (Vector3::Right);
	AddGameObject <Asteroid>()->transform.SetPosition (Vector3::Left);
	AddGameObject <Asteroid>()->transform.SetPosition (Vector3::Up);
	AddGameObject <Asteroid>()->transform.SetPosition (Vector3::Down);
}
