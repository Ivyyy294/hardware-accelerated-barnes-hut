#include "pch.h"
#include "CSUpfrontHistogram.h"

CsUpfrontHistogram::CsUpfrontHistogram()
	: ComputeShader(L"CSUpfrontHistogram.hlsl")
{
}

void CsUpfrontHistogram::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_srvMortonCodesBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavGlobalHistogramBuffer, nullptr);
}

void CsUpfrontHistogram::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsUpfrontHistogram::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsUpfrontHistogram::ReleaseShaderParameters()
{
}
