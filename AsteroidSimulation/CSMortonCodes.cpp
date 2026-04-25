#include "pch.h"
#include "CSMortonCodes.h"
#include "ShaderUtils.h"

CSMortonCodes::CSMortonCodes()
	: ComputeShader(L"CSMortonCodes.hlsl")
{
}

void CSMortonCodes::SetShaderParameter(XMFLOAT3 boundsMin, XMFLOAT3 boundsMax)
{
	ID3D11DeviceContext* m_deviceContext = D3D::Instance().GetDeviceContext();

	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedUnitCube;

	HRESULT result = m_deviceContext->Map(m_unitCubeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedUnitCube);

	if (FAILED(result))
		return;

	UnitCube* unitCube = (UnitCube*)mappedUnitCube.pData;
	unitCube->boundsMax = boundsMax;
	unitCube->boundsMin = boundsMin;

	m_deviceContext->Unmap(m_unitCubeBuffer, 0);
}

void CSMortonCodes::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(0, 1, &m_settingsBuffer);
	deviceContext->CSSetConstantBuffers(1, 1, &m_unitCubeBuffer);
	deviceContext->CSSetShaderResources(0, 1, &m_srvPositionBuffer);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_mortonCodesBufferUav, nullptr);
}

void CSMortonCodes::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);
	deviceContext->CSSetConstantBuffers(1, 1, &nullBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

bool CSMortonCodes::InitShaderParameters(ID3D11Device* device)
{
	HRESULT result;
	return ShaderUtils::CreateConstBuffer (device, result, &m_unitCubeBuffer, sizeof(UnitCube));
}

void CSMortonCodes::ReleaseShaderParameters()
{
	ShaderUtils::SafeRelease(&m_unitCubeBuffer);
}
