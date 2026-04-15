#pragma once

#include "BaseAsteroidScene.h"
#include "ForceGeneratorBarnesHut.h"

class SceneBarnesHut
: public BaseAsteroidScene
{
//Variables
private:
	ForceGeneratorBarnesHut* m_forceGenerator {nullptr};

//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
	void AdjustFixedUpdateLoop();
};

