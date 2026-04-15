#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"

#define MaxCapacity 1024

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	BarnesHutCS m_computeShader;

	std::array<int, MaxCapacity> m_mortonKey;

//Methods
public:

	void Start() override;
	void Exit() override;
	void FixedUpdate() override;
	void ApplyForceForObject(PhysicObject* physicObj) const override;
	void Dispatch();
	void ReadBack();

private:
	float Normalize(float val, float max);
	int Quantize (float val, const int depth);
	uint64_t GetMortonCode64 (uint64_t val);
	unsigned long GetMortonCode32 (unsigned long val);
};

