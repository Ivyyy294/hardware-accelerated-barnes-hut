#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"
#include "CSSettings.h"
#include "CSMortonCodes.h"
#include "CSUpfrontHistogram.h"
#include "CSExclusiveSum.h"
#include "CSDigitBinning.h"
#include "CS_CornerStone_F.h"

#include "CsSharedBuffer.h"

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext {nullptr};
	
	//Test staging buffer
	CsSharedBuffer m_buffer;

	//Compute shader kernel
	BarnesHutCS m_computeShader;
	CSMortonCodes m_csMortonCodes;
	CSUpfrontHistogram m_csUpfrontHistrogram;
	CSExclusiveSum m_csExclusiveSum;
	CSDigitBinning m_csDigitBinning;
	CS_CornerStone_F m_csCornerStone_F;

	XMFLOAT3 m_boundsMin {0.f, 0.f, 0.f};
	XMFLOAT3 m_boundsMax { 0.f, 0.f, 0.f };

//Methods
public:
	ForceGeneratorBarnesHutCS();

	void Start() override;
	void Exit() override;
	void ApplyForceForObject(PhysicObject* physicObj) const override;
	void Dispatch();
	void ReadBack();

private:
	void SetShaderParameter();

	void CallculateMortonKeys (uint32_t elementCount);
	void DispatchRadixSort (uint32_t elementCount);
	void BuildOctree (uint32_t elementCount);
	void CallculateForces (uint32_t elementCount);

	void CopyTestData();
};

