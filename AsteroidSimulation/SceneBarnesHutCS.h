#pragma once

#include "BaseAsteroidScene.h"
#include "ForceGeneratorBarnesHutCS.h"

class SceneBarnesHutCS :
    public BaseAsteroidScene
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

