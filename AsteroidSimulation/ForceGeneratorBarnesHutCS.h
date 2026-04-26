#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"
#include "CSSettings.h"
#include "CSMortonCodes.h"
#include "CSUpfrontHistogram.h"
#include "CSExclusiveSum.h"
#include "CSDigitBinning.h"

#define MaxCapacity 1024

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext {nullptr};

	//Settings buffer
	ID3D11Buffer* m_settingsBuffer{ nullptr };

	//Morton Code Buffer
	ID3D11Buffer* m_upMortonCodeBuffer{ nullptr };
	ID3D11Buffer* m_inMortonCodeBuffer{ nullptr };
	ID3D11Buffer* m_outMortonCodeBuffer{ nullptr };
	ID3D11ShaderResourceView* m_srvMortonCodeBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavMortonCodeBuffer = nullptr;

	//Position Buffer
	ID3D11Buffer* m_upPositionBuffer{ nullptr };
	ID3D11Buffer* m_inPositionBuffer{ nullptr };
	ID3D11Buffer* m_outPositionBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavPositionBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvPositionBuffer = nullptr;

	//Mass Buffer
	ID3D11Buffer* m_upMassBuffer{ nullptr };
	ID3D11Buffer* m_inMassBuffer{ nullptr };
	ID3D11Buffer* m_outMassBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavMassBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvMassBuffer = nullptr;

	//Sorting Buffer
	ID3D11Buffer* m_globalHistogramBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavGlobalHistogramBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvGlobalHistogramBuffer = nullptr;

	ID3D11Buffer* m_exclusivePrefixSumBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavExclusivePrefixSumBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvEclusivePrefixSumBuffer = nullptr;

	//Test staging buffer
	ID3D11Buffer* m_outputBuffer{ nullptr };

	//Compute shader kernel
	BarnesHutCS m_computeShader;
	CSMortonCodes m_csMortonCodes;
	CSUpfrontHistogram m_csUpfrontHistrogram;
	CSExclusiveSum m_csExclusiveSum;
	CSDigitBinning m_csDigitBinning;

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
	void SetTestShaderParameter();
	void SetSettingParameter (uint32_t elementCount, uint32_t dispatchGroupCount, uint32_t digitPlace = 0);

	void CallculateMortonKeys (uint32_t elementCount);
	void DispatchRadixSort (uint32_t elementCount);
	void BuildOctree (uint32_t elementCount);
	void CallculateForces (uint32_t elementCount);

	HRESULT InitMortonCodeShaderParameters (ID3D11Device* device);
	HRESULT InitPositionShaderParameters (ID3D11Device* device);
	HRESULT InitMassShaderParameters (ID3D11Device* device);
	HRESULT InitSortingShaderParameters (ID3D11Device* device);
	bool InitShaderParameters();
	void ReleaseShaderParameters();

	void CopyTestData();
};

