#pragma once

#include "SceneAsteroidBase.h"
#include "ForceGeneratorBarnesHut.h"

class SceneBarnesHut
: public SceneAsteroidBase
{
//Variables
private:
	ForceGeneratorBarnesHut* m_forceGenerator {nullptr};

//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

