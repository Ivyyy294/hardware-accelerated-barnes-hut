#pragma once
#include "ColorMaterial.h"
#include<IvyyyGameObject.h>
#include "StellarObject.h"

class Planet :
    public StellarObject
{
//variables
	std::shared_ptr<ColorMaterial> m_material = nullptr;

//Methods
public:
	Planet();
	void SetData(const Color& color, const float mass, const float size);
};

