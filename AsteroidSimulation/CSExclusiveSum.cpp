#include "pch.h"
#include "CSExclusiveSum.h"

CSExclusiveSum::CSExclusiveSum()
	: ComputeShader(L"CSExclusiveSum.hlsl")
{
}

void CSExclusiveSum::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_globalHistogramBufferUav, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &m_exclusivePrefixSumBufferUav, nullptr);
}

void CSExclusiveSum::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CSExclusiveSum::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CSExclusiveSum::ReleaseShaderParameters()
{
}
