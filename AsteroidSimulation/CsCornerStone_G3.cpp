#include "pch.h"
#include "CsCornerStone_G3.h"

CsCornerStone_G3::CsCornerStone_G3()
	: ComputeShader(L"CSCornerStone_G3.hlsl")
{
}

void CsCornerStone_G3::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_srvKBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_srvO1Buffer);
	deviceContext->CSSetShaderResources(2, 1, &m_srvO2Buffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavKBuffer, nullptr);
}

void CsCornerStone_G3::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G3::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G3::ReleaseShaderParameters()
{
}
