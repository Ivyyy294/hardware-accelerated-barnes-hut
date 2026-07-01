#pragma once

#include <IvyyyScene.h>
#include "Asteroid.h"
#include "SessionVariables.h"
#include "ForceGeneratorBase.h"
#include "PlanetData.h"

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
	void AdjustFixedUpdateLoop (ForceGeneratorBase* forceGenerator);

private:
	void SpawnAsteroids(int num, PhysicObject* soi);
	void SpawnPlanet (const PlanetData& pData, PhysicObject* soi);
};

