#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsBarnesHut_CF :
    public ComputeShader
{
//Variables
public:
	CsSharedBuffer* m_sharedBuffer = nullptr;

//Method
public:
	CsBarnesHut_CF();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

