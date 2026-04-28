#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsCornerStone_G1 :
    public ComputeShader
{
//Variable
public:
	
	CsSharedBuffer* m_sharedBuffer {nullptr};

//Methods
public:
	CsCornerStone_G1();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

