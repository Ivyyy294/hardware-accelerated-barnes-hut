#include "pch.h"
#include "CsCornerStone_G1.h"

CsCornerStone_G1::CsCornerStone_G1()
	: ComputeShader (L"CSCornerStone_G1.hlsl")
{
}

void CsCornerStone_G1::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers (0, 1, &m_settingsBuffer);
	deviceContext->CSSetShaderResources (0, 1, &m_srvKBuffer);
	deviceContext->CSSetShaderResources (1, 1, &m_srvNBuffer);
	deviceContext->CSSetUnorderedAccessViews (0, 1, &m_uavO1Buffer, nullptr);
}

void CsCornerStone_G1::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers (0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G1::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G1::ReleaseShaderParameters()
{
}
