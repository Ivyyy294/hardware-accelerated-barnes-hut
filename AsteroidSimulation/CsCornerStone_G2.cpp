#include "pch.h"
#include "CsCornerStone_G2.h"

CsCornerStone_G2::CsCornerStone_G2()
	: ComputeShader (L"CSCornerStone_G2.hlsl")
{
}

void CsCornerStone_G2::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_o1.m_srv);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_srvCounterBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_o2.m_uav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsCornerStone_G2::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G2::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G2::ReleaseShaderParameters()
{
}
