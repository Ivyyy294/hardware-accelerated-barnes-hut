#pragma once

#include <IvyyyComputeShader.h>

class CsCornerStone_G2 :
    public ComputeShader
{
//Variables
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvOBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavOBuffer = nullptr;

//Methods
public:
	CsCornerStone_G2();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

