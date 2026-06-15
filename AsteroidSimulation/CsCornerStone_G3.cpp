#include "pch.h"
#include "CsCornerStone_G3.h"

CsCornerStone_G3::CsCornerStone_G3()
	: ComputeShader(L"CSCornerStone_G3.hlsl")
{
}

void CsCornerStone_G3::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvKBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_o1.m_srv);
	deviceContext->CSSetShaderResources(2, 1, &m_sharedBuffer->m_o2.m_srv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavKBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_sharedBuffer->m_uavCounterBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsCornerStone_G3::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G3::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G3::ReleaseShaderParameters()
{
}
