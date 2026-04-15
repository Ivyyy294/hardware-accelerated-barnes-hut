#pragma once

#include "SceneAsteroidBase.h"
#include "ForceGeneratorBarnesHutCS.h"

class SceneBarnesHutCS :
    public SceneAsteroidBase
{
//Variables
private:
	ForceGeneratorBarnesHutCS* m_forceGenerator {nullptr};
	
//Methods
private:
	void Init() override;
	void AdjustFixedUpdateLoop();
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

