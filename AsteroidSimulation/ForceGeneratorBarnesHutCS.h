#pragma once

#include "ForceGeneratorBarnesHutBase.h"
#include "BarnesHutCS.h"
#include "CSMortonCodes.h"

#define MaxCapacity 1024

class ForceGeneratorBarnesHutCS :
    public ForceGeneratorBarnesHutBase
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext {nullptr};

	ID3D11Buffer* m_positionBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_positionBufferUav = nullptr;

	ID3D11Buffer* m_mortonCodeBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_mortonCodeBufferUav = nullptr;

	BarnesHutCS m_computeShader;
	CSMortonCodes m_csMortonCodes;

	XMFLOAT3 m_boundsMin;
	XMFLOAT3 m_boundsMax;

	std::array<int, MaxCapacity> m_mortonKey;

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

	bool InitShaderParameters();
	void ReleaseShaderParameters();
};

