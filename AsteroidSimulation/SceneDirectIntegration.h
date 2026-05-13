#pragma once

#include "SceneAsteroidBase.h"
#include "ForceGeneratorDirect.h"

using namespace Ivyyy;

class SceneDirectIntegration :
    public SceneAsteroidBase
{
//variables
private:
	ForceGeneratorDirect* m_forceGenerator{ nullptr };

//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

