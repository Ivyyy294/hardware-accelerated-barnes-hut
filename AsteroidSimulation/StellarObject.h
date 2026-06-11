#pragma once

#include "OrbitalElement.h"

class StellarObject :
    public GameObject
{
	//variables
protected:
	PhysicObject* m_physicObject{ nullptr };

	//Methods
public:
	StellarObject();

	void SetPhysicalProperties(float mass, float size);
	void SetOrbit(OrbitalElement oe, const PhysicObject* soi);
};

