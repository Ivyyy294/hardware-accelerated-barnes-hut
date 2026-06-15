#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsRadixSort_3 :
    public ComputeShader
{
//Variables
public:
	CsSharedBuffer* m_sharedBuffer = nullptr;

	ID3D11UnorderedAccessView* m_uavKeyBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKeyBuffer = nullptr;

	ID3D11UnorderedAccessView* m_uavVal1Buffer = nullptr;
	ID3D11UnorderedAccessView* m_uavVal2Buffer = nullptr;

	ID3D11ShaderResourceView* m_srvVal1Buffer = nullptr;
	ID3D11ShaderResourceView* m_srvVal2Buffer = nullptr;

//Methods
public:
	CsRadixSort_3();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

