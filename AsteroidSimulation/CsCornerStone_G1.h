#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsCornerStone_G1 :
    public ComputeShader
{
//Variable
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvNBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCounterBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavO1Buffer = nullptr;

//Methods
public:
	CsCornerStone_G1();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

