#include "pch.h"
#include "CSDigitBinning.h"

CSDigitBinning::CSDigitBinning()
	: ComputeShader(L"CSDigitBinning.hlsl")
{
}

void CSDigitBinning::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_positionBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_exclusivePrefixSumBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &m_mortonCodesBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &m_positionBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(4, 1, &m_massBufferUav, nullptr);
}

void CSDigitBinning::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(4, 1, &nullUAV, nullptr);
}

bool CSDigitBinning::InitShaderParameters(ID3D11Device* device)
{
    return false;
}

void CSDigitBinning::ReleaseShaderParameters()
{
}
