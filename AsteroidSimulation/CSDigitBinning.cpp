#include "pch.h"
#include "CSDigitBinning.h"
#include "ShaderUtils.h"

CsDigitBinning::CsDigitBinning()
	: ComputeShader(L"CSDigitBinning.hlsl")
{
}

void CsDigitBinning::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	UINT clearValues[4] = { 0, 0, 0, 0 };
	deviceContext->ClearUnorderedAccessViewUint(m_uavScatterCountersBuffer, clearValues);

	deviceContext->CSSetConstantBuffers (0, 1, &m_settingsBuffer);

	deviceContext->CSSetShaderResources (0, 1, &m_exclusivePrefixSumBufferSrv);
	deviceContext->CSSetShaderResources (1, 1, &m_srvMortonCodesBuffer);
	deviceContext->CSSetShaderResources (2, 1, &m_srvPositionBuffer);
	deviceContext->CSSetShaderResources (3, 1, &m_srvMassBuffer);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavScatterCountersBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_uavMortonCodesBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &m_uavPositionBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &m_uavMassBuffer, nullptr);
}

void CsDigitBinning::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);

	deviceContext->CSGetShaderResources(0, 1, &nullSRV);
	deviceContext->CSGetShaderResources(1, 1, &nullSRV);
	deviceContext->CSGetShaderResources(2, 1, &nullSRV);
	deviceContext->CSGetShaderResources(3, 1, &nullSRV);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
}

bool CsDigitBinning::InitShaderParameters(ID3D11Device* device)
{
	HRESULT result;
	bool ok = ShaderUtils::CreateResourceBuffer (device, result, &m_scatterCountersBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateUAVBuffer (device, result, m_scatterCountersBuffer, &m_uavScatterCountersBuffer, 1024);

	return ok;
}

void CsDigitBinning::ReleaseShaderParameters()
{
	ShaderUtils::SafeRelease(&m_scatterCountersBuffer);
	ShaderUtils::SafeRelease(&m_uavScatterCountersBuffer);
}
