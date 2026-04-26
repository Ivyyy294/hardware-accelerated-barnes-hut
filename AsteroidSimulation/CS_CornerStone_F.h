#pragma once

#include <IvyyyComputeShader.h>

class CS_CornerStone_F
	 : public ComputeShader
{
//Variables
public:
	ID3D11Buffer* m_settingsBuffer = nullptr;
	ID3D11ShaderResourceView* m_mortonKeyBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavNBuffer = nullptr;

//Methods
	public:
	CS_CornerStone_F();
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

