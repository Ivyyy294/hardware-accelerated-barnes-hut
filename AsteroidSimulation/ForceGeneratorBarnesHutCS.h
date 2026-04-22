#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"
#include "CSSettings.h"
#include "CSMortonCodes.h"
#include "CSUpfrontHistogram.h"
#include "CSExclusiveSum.h"

#define MaxCapacity 1024

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext {nullptr};

	//Settings buffer
	ID3D11Buffer* m_settingsBuffer{ nullptr };

	//Particle info Buffer
	ID3D11Buffer* m_positionUploadBuffer{ nullptr };
	ID3D11Buffer* m_positionBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_positionBufferUav = nullptr;

	//Additional Buffer
	ID3D11Buffer* m_mortonCodeBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_mortonCodeBufferUav = nullptr;

	//Sorting Buffer
	ID3D11Buffer* m_globalHistogramBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_globalHistogramBufferUav = nullptr;

	ID3D11Buffer* m_exclusivePrefixSumBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_exclusivePrefixSumBufferUav = nullptr;

	//Test staging buffer
	ID3D11Buffer* m_outputBuffer{ nullptr };

	//Compute shader kernel
	BarnesHutCS m_computeShader;
	CSMortonCodes m_csMortonCodes;
	CSUpfrontHistogram m_csUpfrontHistrogram;
	CSExclusiveSum m_csExclusiveSum;

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

	HRESULT InitPositionShaderParameters (ID3D11Device* device);
	HRESULT InitSortingShaderParameters (ID3D11Device* device);
	bool InitShaderParameters();
	void ReleaseShaderParameters();

	void CopyTestData();
};

