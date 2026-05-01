#include "pch.h"
#include "CsBarnesHut_MP.h"

CsBarnesHut_MP::CsBarnesHut_MP()
	: ComputeShader (L"CSBarnesHut_MP.hlsl")
{
}

void CsBarnesHut_MP::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvMortonCodeBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_srvPositionBuffer);
	deviceContext->CSSetShaderResources(2, 1, &m_sharedBuffer->m_srvMassBuffer);

	deviceContext->CSSetShaderResources(3, 1, &m_sharedBuffer->m_srvKBuffer);
	deviceContext->CSSetShaderResources(4, 1, &m_sharedBuffer->m_srvNkBuffer);
	deviceContext->CSSetShaderResources(5, 1, &m_sharedBuffer->m_srvLoBuffer);
	deviceContext->CSSetShaderResources(6, 1, &m_sharedBuffer->m_srvCoBuffer);

	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsBarnesHut_MP::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetShaderResources(3, 1, &nullSRV);
	deviceContext->CSSetShaderResources(4, 1, &nullSRV);
	deviceContext->CSSetShaderResources(5, 1, &nullSRV);
	deviceContext->CSSetShaderResources(6, 1, &nullSRV);
}

bool CsBarnesHut_MP::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsBarnesHut_MP::ReleaseShaderParameters()
{
}
