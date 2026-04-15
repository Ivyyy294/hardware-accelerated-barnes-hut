#pragma once

#include <IvyyyForceGenerator.h>
#include "BarnesHutCS.h"

class ForceGeneratorBarnesHutCS :
    public ForceGenerator
{
//Variables
private:
	float m_theta = 0.8f;
	std::vector<PhysicObject*> m_objects;
	BarnesHutCS m_computeShader;

//Methods
public:
	void Start() override;
	void Exit() override;
	void FixedUpdate() override;
	void ApplyForceForObject(PhysicObject* physicObj) const override;
	void Dispatch();
	void ReadBack();

	inline void AddForceSource(PhysicObject* obj) { m_objects.push_back(obj); };
};

