#pragma once

#include <IvyyyForceGenerator.h>

class ForceGeneratorBase :
    public ForceGenerator
{
//Variables
protected:
	float m_theta = 0.8f;
	std::vector<PhysicObject*> m_objects;
	XMFLOAT3* m_forceBuffer = nullptr;
	uint32_t m_maxParticleCount = 0;
//Methods
public:
	ForceGeneratorBase ();
	~ForceGeneratorBase();

	inline void AddForceSource(PhysicObject* obj) { m_objects.push_back(obj); };
	void ApplyForceForObject(PhysicObject* physicObj) const override final;
	void FixedUpdate() override;
	void PreCalculateForces();
protected:
	virtual void PreCalculateForcesInternal() = 0;
	virtual void DoPostWork(){};
	float GetMaxCoordinate();
};

