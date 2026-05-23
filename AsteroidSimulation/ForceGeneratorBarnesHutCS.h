#pragma once

#include "ForceGeneratorBase.h"

#include "CsSharedBuffer.h"
#include "CsSettings.h"

#include "CsMortonCodes.h"

#include "CsRadixSort_1.h"
#include "CsRadixSort_2.h"
#include "CsRadixSort_3.h"
#include "CsRadixSort_4.h"

#include "CsCornerStone_F.h"
#include "CsCornerStone_G1.h"
#include "CsCornerStone_G2.h"
#include "CsCornerStone_G3.h"
#include "CsCornerStone_NK.h"
#include "CsCornerStone_LO.h"
#include "CsCornerStone_CO.h"

#include "CsBarnesHut_MP.h"
#include "CsBarnesHut_CF.h"

#include "ShaderUtils.h"

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBase
{
//Variables
private:
	const uint32_t m_nCrit {128};

	ID3D11DeviceContext* m_deviceContext {nullptr};
	
	//Test staging buffer
	CsSharedBuffer m_buffer;

	//Compute shader kernel
	CsMortonCodes m_csMortonCodes;

	CsRadixSort_1 m_csRadixSort_1;
	CsRadixSort_2 m_csRadixSort_2;
	CsRadixSort_3 m_csRadixSort_3;
	CsRadixSort_4 m_csRadixSort_4;

	CsCornerStone_F m_csCornerStone_F;
	CsCornerStone_G1 m_csCornerStone_G1;
	CsCornerStone_G2 m_csCornerStone_G2;
	CsCornerStone_G3 m_csCornerStone_G3;
	CsCornerStone_NK m_csCornerStone_NK;
	CsCornerStone_LO m_csCornerStone_LO;
	CsCornerStone_CO m_csCornerStone_CO;

	CsBarnesHut_MP m_csBarnesHut_MP;
	CsBarnesHut_CF m_csBarnesHut_CF;

	float m_maxCoord = 0.f;
	bool m_dumpMemmory = false;
	uint32_t m_counterBuffer[3];

//Methods
public:
	ForceGeneratorBarnesHutCS();

	void Start() override;
	void Exit() override;
	void Update() override;

private:
	void PreCalculateForcesInternal() override;
	void SetShaderParameter();

	void CallculateMortonKeys (uint32_t elementCount);
	void DispatchRadixSort (uint32_t elementCount);
	void BuildOctree (uint32_t elementCount);
	void CallculateForces (uint32_t elementCount);

	void CopyTestData();
	void CopyForceData();
	void CopyCounterBuffer();

	void RecordExecutionTime();

};

