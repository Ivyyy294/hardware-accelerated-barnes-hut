#pragma once

#include <IvyyyGameObject.h>
#include <IvyyyMesh.h>
#include "ColorMaterial.h"
#include "OrbitalElement.h"
#include "StellarObject.h"

using namespace Ivyyy;

class Asteroid :
    public StellarObject
{
//variables
private:
	static std::shared_ptr<ColorMaterial> m_material;
	static Mesh m_mesh;

//Methods
public:
	Asteroid();
};

