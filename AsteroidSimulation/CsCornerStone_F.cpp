#include "pch.h"
#include "CsCornerStone_F.h"

CsCornerStone_F::CsCornerStone_F()
	: ComputeShader (L"CSCornerStone_F.hlsl")
{
}

void CsCornerStone_F::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers (0, 1, &m_settingsBuffer);

	deviceContext->CSSetShaderResources (0, 1, &m_mortonKeyBuffer);
	deviceContext->CSSetShaderResources (1, 1, &m_srvKBuffer);

	UINT clearValues[4] = { 0, 0, 0, 0 };
	deviceContext->ClearUnorderedAccessViewUint(m_uavNBuffer, clearValues);

	deviceContext->CSSetUnorderedAccessViews (0, 1, &m_uavNBuffer, nullptr);
}

void CsCornerStone_F::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers (0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
}

bool CsCornerStone_F::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_F::ReleaseShaderParameters()
{
}
