#include "pch.h"
#include "CsRadixSort_4.h"
#include "ShaderUtils.h"

CsRadixSort_4::CsRadixSort_4()
	: ComputeShader(L"CsRadixSort_4.hlsl")
{
}

void CsRadixSort_4::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_sharedBuffer->m_settingsBuffer);

	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_exclusivePrefixSum.m_srv);
	deviceContext->CSSetShaderResources(1, 1, &m_srvKeyBuffer);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavKeyBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsRadixSort_4::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);

	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &nullUAV, nullptr);
}

bool CsRadixSort_4::InitShaderParameters(ID3D11Device* device)
{
	return true;
}

void CsRadixSort_4::ReleaseShaderParameters()
{
}
