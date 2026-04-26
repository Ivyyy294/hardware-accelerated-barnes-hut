#pragma once

#include <IvyyyScene.h>
#include "Asteroid.h"

class SceneAsteroidBase :
    public Scene
{
//Methods
protected:
	void Init() override;

	virtual void OnOrbitalBodySpawned(GameObject* orbitalBody) = 0;

private:
	void SpawnAsteroids(int num, float dist, float speed);
	void SpawnAsteroids(int num, Vector2 dist, Vector2 e, Vector2 i, GameObject* planet);
};

