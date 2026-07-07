#pragma once
#include "ColorMaterial.h"
#include "StellarObject.h"
#include "PlanetData.h"
#include "PlanetRing.h"

class Planet :
    public StellarObject
{
//variables
private:
	std::shared_ptr<ColorMaterial> m_material = nullptr;
	std::shared_ptr <PlanetRing> m_ring;

//Methods
public:
	Planet();
	void SetData(const PlanetData& data);
};

