#include "pch.h"
#include "CsCornerStone_LO.h"

CsCornerStone_LO::CsCornerStone_LO()
	: ComputeShader(L"CSCornerStone_LO.hlsl")
{
}

void CsCornerStone_LO::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvCounterBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_srvNkBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavLoBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsCornerStone_LO::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	deviceContext->CSGetShaderResources(0, 1, &nullSRV);
	deviceContext->CSGetShaderResources(1, 1, &nullSRV);
	deviceContext->CSGetUnorderedAccessViews(0, 1, &nullUAV);
	deviceContext->CSGetUnorderedAccessViews(1, 1, &nullUAV);
}

bool CsCornerStone_LO::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_LO::ReleaseShaderParameters()
{
}
