#include "pch.h"
#include "CSBuildOctree.h"
#include "ShaderUtils.h"

void CSBuildOctree::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	//UINT clearValues[4] = { 0, 0, 0, 0 };
	//deviceContext->ClearUnorderedAccessViewUint(m_uavScatterCountersBuffer, clearValues);

	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);

	deviceContext->CSSetShaderResources(0, 1, &m_srvMortonCodesBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_srvPositionBuffer);
	deviceContext->CSSetShaderResources(2, 1, &m_srvMassBuffer);

	//deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uavScatterCountersBuffer, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(1, 1, &m_uavMortonCodesBuffer, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(2, 1, &m_uavPositionBuffer, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(3, 1, &m_uavMassBuffer, nullptr);
}

void CSBuildOctree::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);

	deviceContext->CSGetShaderResources(0, 1, &nullSRV);
	deviceContext->CSGetShaderResources(1, 1, &nullSRV);
	deviceContext->CSGetShaderResources(2, 1, &nullSRV);
	//deviceContext->CSGetShaderResources(3, 1, &nullSRV);

	//deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
}

bool CSBuildOctree::InitShaderParameters(ID3D11Device* device)
{
    return false;
}

void CSBuildOctree::ReleaseShaderParameters()
{
}
