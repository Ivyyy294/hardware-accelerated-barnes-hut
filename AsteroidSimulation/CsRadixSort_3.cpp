#include "pch.h"
#include "CsRadixSort_3.h"
#include "ShaderUtils.h"

CsRadixSort_3::CsRadixSort_3()
	: ComputeShader(L"CSRadixSort_3.hlsl")
{
}

void CsRadixSort_3::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	UINT clearValues[4] = { 0, 0, 0, 0 };
	deviceContext->ClearUnorderedAccessViewUint(m_uavScatterCountersBuffer, clearValues);

	deviceContext->CSSetConstantBuffers (0, 1, &m_sharedBuffer->m_settingsBuffer);

	deviceContext->CSSetShaderResources (0, 1, &m_sharedBuffer->m_srvEclusivePrefixSumBuffer);

	deviceContext->CSSetShaderResources (1, 1, &m_srvKeyBuffer);
	deviceContext->CSSetShaderResources (2, 1, &m_srvVal1Buffer);
	deviceContext->CSSetShaderResources (3, 1, &m_srvVal2Buffer);
	deviceContext->CSSetShaderResources (4, 1, &m_sharedBuffer->m_srvNewToOldIndexBuffer);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavScatterCountersBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_uavKeyBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &m_uavVal1Buffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &m_uavVal2Buffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(4, 1, &m_sharedBuffer->m_uavNewToOldIndexBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(5, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsRadixSort_3::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);

	deviceContext->CSGetShaderResources(0, 1, &nullSRV);
	deviceContext->CSGetShaderResources(1, 1, &nullSRV);
	deviceContext->CSGetShaderResources(2, 1, &nullSRV);
	deviceContext->CSGetShaderResources(3, 1, &nullSRV);
	deviceContext->CSGetShaderResources(4, 1, &nullSRV);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(4, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(5, 1, &nullUAV, nullptr);
}

bool CsRadixSort_3::InitShaderParameters(ID3D11Device* device)
{
	HRESULT result;
	bool ok = ShaderUtils::CreateResourceBuffer (device, result, &m_scatterCountersBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateUAVBuffer (device, result, m_scatterCountersBuffer, &m_uavScatterCountersBuffer, 1024);

	return ok;
}

void CsRadixSort_3::ReleaseShaderParameters()
{
	ShaderUtils::SafeRelease(&m_scatterCountersBuffer);
	ShaderUtils::SafeRelease(&m_uavScatterCountersBuffer);
}
