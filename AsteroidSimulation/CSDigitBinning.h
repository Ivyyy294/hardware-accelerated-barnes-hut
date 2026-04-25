#pragma once

#include <IvyyyComputeShader.h>

class CSDigitBinning :
    public ComputeShader
{
//Variables
public:

	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11UnorderedAccessView* m_mortonCodesBufferUav = nullptr;
	ID3D11ShaderResourceView* m_mortonCodesBufferSrv = nullptr;
	ID3D11ShaderResourceView* m_exclusivePrefixSumBufferSrv = nullptr;


	//ID3D11UnorderedAccessView* m_uavPositionBuffer = nullptr;
	//ID3D11UnorderedAccessView* m_uavMassBuffer = nullptr;
private:
	ID3D11Buffer* m_scatterCountersBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavScatterCountersBuffer = nullptr;

//Methods
public:
	CSDigitBinning();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

