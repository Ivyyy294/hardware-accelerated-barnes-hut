#include "pch.h"
#include "CsCornerStone_G1.h"

CsCornerStone_G1::CsCornerStone_G1()
	: ComputeShader (L"CSCornerStone_G1.hlsl")
{
}

void CsCornerStone_G1::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers (0, 1, &m_sharedBuffer->m_settingsBuffer);
	deviceContext->CSSetShaderResources (0, 1, &m_sharedBuffer->m_srvKBuffer);
	deviceContext->CSSetShaderResources (1, 1, &m_sharedBuffer->m_srvNBuffer);
	deviceContext->CSSetShaderResources (2, 1, &m_sharedBuffer->m_srvCounterBuffer);
	deviceContext->CSSetUnorderedAccessViews (0, 1, &m_sharedBuffer->m_uavO1Buffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews (1, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsCornerStone_G1::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers (0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CsCornerStone_G1::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_G1::ReleaseShaderParameters()
{
}
