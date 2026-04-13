#pragma once

#include "BaseAsteroidScene.h"

using namespace Ivyyy;

class DirectIntegrationScene :
    public BaseAsteroidScene
{
//Methods
private:
	void Init() override;
	void OnOrbitalBodySpawned(GameObject* orbitalBody) override;
};

