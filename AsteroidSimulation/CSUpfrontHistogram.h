#pragma once

#include <IvyyyComputeShader.h>

class CSUpfrontHistogram :
    public ComputeShader
{
//Variables
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvMortonCodesBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavGlobalHistogramBuffer = nullptr;
//Methods
public:
	CSUpfrontHistogram();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

