#pragma once

#include "SpatialOctree.h"

class BarnesHutForceGenerator
	: public ForceGenerator
{
//Variables
private:
	float m_theta = 0.8f;

	std::vector<PhysicObject*> m_objects;

	//ToDo callculate init size
	SpatialOctree m_octTree {Vector3::Zero, {50.f, 50.f, 50.f}};

//Methods
public:
	inline void AddForceSource (PhysicObject* obj) {m_objects.push_back(obj);};

	void OnDrawGizmo() override;

	// Inherited via ForceGenerator
	void ApplyForceForObject(PhysicObject* physicObj) const override;

	void BuildTree();
};

