#pragma once

#include <IvyyyForceGenerator.h>
#include "BarnesHutCS.h"

class BarnesHutCSForceGenerator :
    public ForceGenerator
{
//Variables
private:
	BarnesHutCS m_computeShader;
//Methods
public:
	void Start() override;
	void Exit() override;
	void FixedUpdate() override;
	void ApplyForceForObject(PhysicObject* physicObj) const override;
};

