#pragma once

#include <IvyyyForceGenerator.h>

class ForceGeneratorBase :
    public ForceGenerator
{
//Variables
protected:
	float m_theta = 0.8f;
	std::vector<PhysicObject*> m_objects;
	XMFLOAT3 m_forceBuffer[MAX_PARTICLE_COUNT]{};

//Methods
public:
	inline void AddForceSource(PhysicObject* obj) { m_objects.push_back(obj); };
	void ApplyForceForObject(PhysicObject* physicObj) const override final;
	void FixedUpdate() override;
	void PreCalculateForces();
protected:
	virtual void PreCalculateForcesInternal() = 0;
	float GetMaxCoordinate();
};

