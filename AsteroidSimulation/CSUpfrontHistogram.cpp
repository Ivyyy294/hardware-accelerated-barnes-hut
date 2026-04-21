#include "pch.h"
#include "CSUpfrontHistogram.h"

CSUpfrontHistogram::CSUpfrontHistogram()
	: ComputeShader(L"CSUpfrontHistogram.hlsl")
{
}

void CSUpfrontHistogram::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_mortonCodesBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_globalHistogramBufferUav, nullptr);
}

void CSUpfrontHistogram::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CSUpfrontHistogram::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CSUpfrontHistogram::ReleaseShaderParameters()
{
}
