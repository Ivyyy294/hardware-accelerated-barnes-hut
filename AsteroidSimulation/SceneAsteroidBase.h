#pragma once

#include <IvyyyScene.h>
#include "Asteroid.h"
#include "SessionVariables.h"

class SceneAsteroidBase :
    public Scene
{
//Variables
protected:
	SessionVariables m_sVar;

//Methods
public:
	SceneAsteroidBase();

protected:
	void Init() override;

	virtual void OnOrbitalBodySpawned(GameObject* orbitalBody) = 0;

private:
	void SpawnAsteroids(int num, float dist, float speed);
	void SpawnAsteroids(int num, Vector2 dist, Vector2 e, Vector2 i, GameObject* planet);

	void LogSessionVariables();
};

