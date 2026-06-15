#include "pch.h"
#include "CsBarnesHut_CF.h"

CsBarnesHut_CF::CsBarnesHut_CF()
	: ComputeShader (L"CSBarnesHut_CF.hlsl")
{
}

void CsBarnesHut_CF::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_sharedBuffer->m_bhSettingsBuffer);

	deviceContext->CSSetShaderResources(0, 1, &m_sharedBuffer->m_srvPositionBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_sharedBuffer->m_srvMassBuffer);
	deviceContext->CSSetShaderResources(2, 1, &m_sharedBuffer->m_srvNewToOldIndexBuffer);

	deviceContext->CSSetShaderResources(3, 1, &m_sharedBuffer->m_srvNkBuffer);
	deviceContext->CSSetShaderResources(4, 1, &m_sharedBuffer->m_srvCoBuffer);
	deviceContext->CSSetShaderResources(5, 1, &m_sharedBuffer->m_srvCenterMassBuffer);
	deviceContext->CSSetShaderResources(6, 1, &m_sharedBuffer->m_srvCombinedMassBuffer);
	deviceContext->CSSetShaderResources(7, 1, &m_sharedBuffer->m_srvLiBuffer);
	deviceContext->CSSetShaderResources(8, 1, &m_sharedBuffer->m_srvLcBuffer);
	deviceContext->CSSetShaderResources(9, 1, &m_sharedBuffer->m_srvCellGeoDataBuffer);
	
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_sharedBuffer->m_uavForceBuffer, nullptr);
	deviceContext->CSSetUnorderedAccessViews(7, 1, &m_sharedBuffer->m_uavDebugBuffer, nullptr);
}

void CsBarnesHut_CF::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuff = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuff);

	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetShaderResources(3, 1, &nullSRV);
	deviceContext->CSSetShaderResources(4, 1, &nullSRV);
	deviceContext->CSSetShaderResources(5, 1, &nullSRV);
	deviceContext->CSSetShaderResources(6, 1, &nullSRV);
	deviceContext->CSSetShaderResources(7, 1, &nullSRV);
	deviceContext->CSSetShaderResources(8, 1, &nullSRV);
	deviceContext->CSSetShaderResources(9, 1, &nullSRV);
	deviceContext->CSSetShaderResources(10, 1, &nullSRV);
	deviceContext->CSSetShaderResources(11, 1, &nullSRV);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CsBarnesHut_CF::InitShaderParameters(ID3D11Device* device)
{
    return true;
}

void CsBarnesHut_CF::ReleaseShaderParameters()
{
}
