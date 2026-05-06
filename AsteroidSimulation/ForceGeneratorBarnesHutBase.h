#pragma once

#include <IvyyyForceGenerator.h>

class ForceGeneratorBarnesHutBase :
    public ForceGenerator
{
//Variables
protected:
	float m_theta = 0.8f;
	std::vector<PhysicObject*> m_objects;

//Methods
public:
	inline void AddForceSource(PhysicObject* obj) { m_objects.push_back(obj); };

	void FixedUpdate() override;

protected:
	float GetMaxCoordinate();
};

