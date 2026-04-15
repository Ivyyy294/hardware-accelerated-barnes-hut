#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "SpatialOctree.h"

class ForceGeneratorBarnesHut
	: public ForceGeneratorBarnesHutBase
{
//Variables
private:
	//ToDo callculate init size
	SpatialOctree m_octTree {Vector3::Zero, {50.f, 50.f, 50.f}};

//Methods
public:
	void OnDrawGizmo() override;

	// Inherited via ForceGenerator
	void ApplyForceForObject(PhysicObject* physicObj) const override;

	void BuildTree();
};

