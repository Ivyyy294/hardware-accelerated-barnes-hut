#pragma once

#include <IvyyyGameObject.h>
#include <IvyyyMesh.h>
#include "ColorMaterial.h"
#include "OrbitalElement.h"

using namespace Ivyyy;

class Asteroid :
    public GameObject
{
//variables
private:
	static std::shared_ptr<ColorMaterial> m_material;
	static Mesh m_mesh;

	PhysicObject* m_physicObject {nullptr};

//Methods
public:
	Asteroid();

	void SetOrbit (OrbitalElement oe, const PhysicObject* soi);
};

