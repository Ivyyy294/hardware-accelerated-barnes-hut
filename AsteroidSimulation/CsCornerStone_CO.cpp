#include "pch.h"
#include "CsCornerStone_CO.h"

CsCornerStone_CO::CsCornerStone_CO()
	: ComputeShader(L"CsCornerStone_CO.hlsl")
{
}

void CsCornerStone_CO::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvCounterBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_srvNkBuffer);
	deviceContext->CSSetShaderResources(2, 1, &m_sharedBuffer->m_srvLoBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavCoBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsCornerStone_CO::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CsCornerStone_CO::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_CO::ReleaseShaderParameters()
{
}
