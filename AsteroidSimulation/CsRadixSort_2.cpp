#include "pch.h"
#include "CsRadixSort_2.h"

CsRadixSort_2::CsRadixSort_2()
	: ComputeShader(L"CSRadixSort_2.hlsl")
{
}

void CsRadixSort_2::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	UINT clearValues[4] = { 0, 0, 0, 0 };
	deviceContext->ClearUnorderedAccessViewUint(m_sharedBuffer->m_uavExclusivePrefixSumBuffer, clearValues);

	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvGlobalHistogramBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavExclusivePrefixSumBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsRadixSort_2::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &nullUAV, nullptr);
}

bool CsRadixSort_2::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsRadixSort_2::ReleaseShaderParameters()
{
}
