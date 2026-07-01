#pragma once
#include "ColorMaterial.h"
#include "StellarObject.h"
#include "PlanetData.h"

class Planet :
    public StellarObject
{
//variables
	std::shared_ptr<ColorMaterial> m_material = nullptr;

//Methods
public:
	Planet();
	void SetData(const PlanetData& data);
};

