#pragma once

#include <IvyyyComputeShader.h>

class CsExclusiveSum :
    public ComputeShader
{
//Variables
public:
	ID3D11ShaderResourceView* m_srvGlobalHistogramBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavExclusivePrefixSumBuffer = nullptr;

//Methods
public:
	CsExclusiveSum();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

