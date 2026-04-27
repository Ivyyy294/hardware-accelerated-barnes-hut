#pragma once

#include <IvyyyComputeShader.h>

class CsCornerStone_G3 :
    public ComputeShader
{
//Variables
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvO1Buffer = nullptr;
	ID3D11ShaderResourceView* m_srvO2Buffer = nullptr;
	ID3D11UnorderedAccessView* m_uavKBuffer = nullptr;
//Methods
public:
	CsCornerStone_G3();

private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

