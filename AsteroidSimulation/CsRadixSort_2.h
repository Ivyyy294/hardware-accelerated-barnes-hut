#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsRadixSort_2 :
    public ComputeShader
{
//Variables
public:
	CsSharedBuffer* m_sharedBuffer = nullptr;

//Methods
public:
	CsRadixSort_2();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

