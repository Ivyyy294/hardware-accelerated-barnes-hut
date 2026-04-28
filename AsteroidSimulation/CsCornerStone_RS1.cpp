#include "pch.h"
#include "CsCornerStone_RS1.h"

CsCornerStone_RS1::CsCornerStone_RS1()
	: ComputeShader(L"CSUpfrontHistogram.hlsl")
{
}

void CsCornerStone_RS1::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_sharedBuffer->m_settingsBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvNkBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavGlobalHistogramBuffer, nullptr);
}

void CsCornerStone_RS1::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSrv);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

bool CsCornerStone_RS1::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsCornerStone_RS1::ReleaseShaderParameters()
{
}
