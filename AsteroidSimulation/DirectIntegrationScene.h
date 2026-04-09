#pragma once
#include <IvyyyScene.h>

using namespace Ivyyy;

class DirectIntegrationScene :
    public Scene
{
	//Methods
private:
	void Init() override;
	void SpawnAsteroids(int num, float dist, float speed);
};

