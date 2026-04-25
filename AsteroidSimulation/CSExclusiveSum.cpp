#include "pch.h"
#include "CSExclusiveSum.h"

CSExclusiveSum::CSExclusiveSum()
	: ComputeShader(L"CSExclusiveSum.hlsl")
{
}

void CSExclusiveSum::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_srvGlobalHistogramBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavExclusivePrefixSumBuffer, nullptr);
}

void CSExclusiveSum::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CSExclusiveSum::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CSExclusiveSum::ReleaseShaderParameters()
{
}
