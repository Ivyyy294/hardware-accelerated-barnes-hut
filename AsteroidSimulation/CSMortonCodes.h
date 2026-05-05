#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsMortonCodes :
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
	CsSharedBuffer* m_sharedBuffer = nullptr;

private:
	ID3D11Buffer* m_unitCubeBuffer{ nullptr };

//Methods
public:
	CsMortonCodes();

	void SetShaderParameter (XMFLOAT3 boundsMin, XMFLOAT3 boundsMax);

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

