#pragma once

#include <IvyyyComputeShader.h>

class CSExclusiveSum :
    public ComputeShader
{
//Variables
public:
	ID3D11UnorderedAccessView* m_globalHistogramBufferUav = nullptr;
	ID3D11UnorderedAccessView* m_exclusivePrefixSumBufferUav = nullptr;

//Methods
public:
	CSExclusiveSum();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

