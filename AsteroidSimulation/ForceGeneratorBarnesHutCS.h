#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "CsSettings.h"
#include "CsMortonCodes.h"
#include "CsUpfrontHistogram.h"
#include "CsExclusiveSum.h"
#include "CsDigitBinning.h"
#include "CsCornerStone_F.h"

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
	CsMortonCodes m_csMortonCodes;
	CsUpfrontHistogram m_csUpfrontHistrogram;
	CsExclusiveSum m_csExclusiveSum;
	CsDigitBinning m_csDigitBinning;
	CsCornerStone_F m_csCornerStone_F;

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

