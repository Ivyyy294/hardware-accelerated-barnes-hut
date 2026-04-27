#include "pch.h"
#include "CsCornerStone_G2.h"

CsCornerStone_G2::CsCornerStone_G2()
	: ComputeShader (L"CSCornerStone_G2.hlsl")
{
}

void CsCornerStone_G2::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_srvO1Buffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavO2Buffer, nullptr);
}

void CsCornerStone_G2::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G2::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G2::ReleaseShaderParameters()
{
}
