#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	BarnesHutCS m_computeShader;

//Methods
public:

	void Start() override;
	void Exit() override;
	void ApplyForceForObject(PhysicObject* physicObj) const override;
	void Dispatch();
	void ReadBack();
};

