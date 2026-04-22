#pragma once

#include <IvyyyComputeShader.h>

class CSDigitBinning :
    public ComputeShader
{
//Variables
public:
	ID3D11UnorderedAccessView* m_exclusivePrefixSumBufferUav = nullptr;
	ID3D11UnorderedAccessView* m_mortonCodesBufferUav = nullptr;
	ID3D11UnorderedAccessView* m_positionBufferUav = nullptr;
	ID3D11UnorderedAccessView* m_massBufferUav = nullptr;
private:
	ID3D11Buffer* m_tileCounterBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_tileCounterBufferUav = nullptr;

//Methods
public:
	CSDigitBinning();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

