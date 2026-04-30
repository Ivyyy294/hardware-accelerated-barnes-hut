#include "pch.h"
#include "CsRadixSort_1.h"

CsRadixSort_1::CsRadixSort_1()
	: ComputeShader(L"CSRadixSort_1.hlsl")
{
}

void CsRadixSort_1::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	UINT clearValues[4] = { 0, 0, 0, 0 };
	deviceContext->ClearUnorderedAccessViewUint(m_sharedBuffer->m_uavGlobalHistogramBuffer, clearValues);

	deviceContext->CSSetConstantBuffers(0, 1, &m_sharedBuffer->m_settingsBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_srvKeyBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavGlobalHistogramBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsRadixSort_1::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CsRadixSort_1::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsRadixSort_1::ReleaseShaderParameters()
{
}
