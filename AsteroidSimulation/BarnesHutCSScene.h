#pragma once

#include "BaseAsteroidScene.h"
#include "BarnesHutCSForceGenerator.h"

class BarnesHutCSScene :
    public BaseAsteroidScene
{
//Variables
private:
	BarnesHutCSForceGenerator* m_forceGenerator {nullptr};
	
//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

