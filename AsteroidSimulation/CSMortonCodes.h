#pragma once

#include <IvyyyComputeShader.h>

class CSMortonCodes :
    public ComputeShader
{
//Definitions
private:
	struct UnitCube
	{
		XMFLOAT3 boundsMax;
		XMFLOAT3 boundsMin;
		double padding;
	};

//Variables
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11UnorderedAccessView* m_mortonCodesBufferUav = nullptr;
	ID3D11UnorderedAccessView* m_uavPositionBuffer = nullptr;

private:
	ID3D11Buffer* m_unitCubeBuffer{ nullptr };

//Methods
public:
	CSMortonCodes();

	void SetShaderParameter (XMFLOAT3 boundsMin, XMFLOAT3 boundsMax);

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

