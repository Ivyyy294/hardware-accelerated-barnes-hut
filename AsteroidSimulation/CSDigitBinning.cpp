#include "pch.h"
#include "CSDigitBinning.h"

CSDigitBinning::CSDigitBinning()
	: ComputeShader(L"CSDigitBinning.hlsl")
{
}

void CSDigitBinning::BindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources (0, 1, &m_exclusivePrefixSumBufferSrv);
	deviceContext->CSSetShaderResources (1, 1, &m_mortonCodesBufferSrv);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_mortonCodesBufferUav, nullptr);

	//deviceContext->CSSetUnorderedAccessViews(0, 1, &m_tileCounterBufferUav, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(1, 1, &m_exclusivePrefixSumBufferUav, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(2, 1, &m_scanStateBufferUav, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(3, 1, &m_mortonCodesBufferUav, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(4, 1, &m_uavPositionBuffer, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(5, 1, &m_uavMassBuffer, nullptr);
}

void CSDigitBinning::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	deviceContext->CSGetShaderResources(0, 1, &nullSRV);
	deviceContext->CSGetShaderResources(1, 1, &nullSRV);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	//deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(4, 1, &nullUAV, nullptr);
	//deviceContext->CSSetUnorderedAccessViews(5, 1, &nullUAV, nullptr);
}

bool CSDigitBinning::InitShaderParameters(ID3D11Device* device)
{
	/*D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.ByteWidth = sizeof(float);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(float);

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_tileCounterBuffer);

	if (FAILED(result))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = 1;

	result = device->CreateUnorderedAccessView(m_tileCounterBuffer, &uavDesc, &m_tileCounterBufferUav);

	if (FAILED(result))
		return false;

	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.ByteWidth = sizeof(uint32_t);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_scanStateBuffer);

	if (FAILED(result))
		return false;

	uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = 1;

	result = device->CreateUnorderedAccessView(m_scanStateBuffer, &uavDesc, &m_scanStateBufferUav);

	return SUCCEEDED(result);*/
	return true;
}

void CSDigitBinning::ReleaseShaderParameters()
{
	//if (m_tileCounterBuffer != nullptr)
	//{
	//	m_tileCounterBuffer->Release();
	//	m_tileCounterBuffer = nullptr;
	//}

	//if (m_tileCounterBufferUav != nullptr)
	//{
	//	m_tileCounterBufferUav->Release();
	//	m_tileCounterBufferUav = nullptr;
	//}
}
