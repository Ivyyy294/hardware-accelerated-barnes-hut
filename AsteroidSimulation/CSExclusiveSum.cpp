#include "pch.h"
#include "CSExclusiveSum.h"

CsExclusiveSum::CsExclusiveSum()
	: ComputeShader(L"CSExclusiveSum.hlsl")
{
}

void CsExclusiveSum::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_srvGlobalHistogramBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavExclusivePrefixSumBuffer, nullptr);
}

void CsExclusiveSum::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsExclusiveSum::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsExclusiveSum::ReleaseShaderParameters()
{
}
