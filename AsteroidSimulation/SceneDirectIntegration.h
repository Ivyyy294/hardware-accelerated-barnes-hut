#pragma once

#include "SceneAsteroidBase.h"

using namespace Ivyyy;

class SceneDirectIntegration :
    public SceneAsteroidBase
{
//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

