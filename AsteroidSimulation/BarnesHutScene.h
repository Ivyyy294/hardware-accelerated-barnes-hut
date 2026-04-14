#pragma once

#include "BaseAsteroidScene.h"
#include "BarnesHutForceGenerator.h"

class BarnesHutScene
: public BaseAsteroidScene
{
//Variables
private:
	BarnesHutForceGenerator* m_forceGenerator {nullptr};

//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
	void AdjustFixedUpdateLoop();
};

