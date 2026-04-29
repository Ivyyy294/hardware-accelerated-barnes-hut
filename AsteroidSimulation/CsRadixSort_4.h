#pragma once

#include <IvyyyComputeShader.h>
#include "CsSharedBuffer.h"

class CsRadixSort_4 :
	public ComputeShader
{
	//Variables
public:
	CsSharedBuffer* m_sharedBuffer = nullptr;

	ID3D11UnorderedAccessView* m_uavKeyBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvKeyBuffer = nullptr;

private:
	ID3D11Buffer* m_scatterCountersBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavScatterCountersBuffer = nullptr;

	//Methods
public:
	CsRadixSort_4();
private:
	void BindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters(ID3D11Device* device) override;
	void ReleaseShaderParameters() override;
};

