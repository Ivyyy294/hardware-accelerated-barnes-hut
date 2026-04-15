#pragma once

#include <IvyyyGameObject.h>
#include "ColorMaterial.h"
#include <IvyyyMesh.h>

using namespace Ivyyy;

class Asteroid :
    public GameObject
{
//variables
private:
	static std::shared_ptr<ColorMaterial> m_material;
	static Mesh m_mesh;
//Methods
public:
	Asteroid();
};

