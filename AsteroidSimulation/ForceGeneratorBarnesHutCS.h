#pragma once

#include "ForceGeneratorBarnesHutBase.h"

#include "CsSharedBuffer.h"
#include "CsSettings.h"
#include "CsMortonCodes.h"
#include "CsUpfrontHistogram.h"
#include "CsExclusiveSum.h"
#include "CsDigitBinning.h"
#include "CsCornerStone_F.h"
#include "CsCornerStone_G1.h"
#include "CsCornerStone_G2.h"
#include "CsCornerStone_G3.h"
#include "CsCornerStone_NK.h"
#include "CsCornerStone_RS1.h"

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	const uint32_t m_nCrit {16};

	ID3D11DeviceContext* m_deviceContext {nullptr};
	
	//Test staging buffer
	CsSharedBuffer m_buffer;

	//Compute shader kernel
	CsMortonCodes m_csMortonCodes;
	CsUpfrontHistogram m_csUpfrontHistrogram;
	CsExclusiveSum m_csExclusiveSum;
	CsDigitBinning m_csDigitBinning;
	CsCornerStone_F m_csCornerStone_F;
	CsCornerStone_G1 m_csCornerStone_G1;
	CsCornerStone_G2 m_csCornerStone_G2;
	CsCornerStone_G3 m_csCornerStone_G3;
	CsCornerStone_NK m_csCornerStone_NK;
	CsCornerStone_RS1 m_csCornerStone_RS1;

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

