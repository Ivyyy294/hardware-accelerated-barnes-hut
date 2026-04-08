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

	Asteroid* obj = AddGameObject <Asteroid>();
}
