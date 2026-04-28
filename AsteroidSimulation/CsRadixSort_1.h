#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsRadixSort_1 :
    public ComputeShader
{
//Variables
public:
	CsSharedBuffer* m_sharedBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKeyBuffer = nullptr;
//Methods
public:
	CsRadixSort_1();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

