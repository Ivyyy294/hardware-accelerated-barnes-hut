#include "pch.h"
#include "BarnesHutCS.h"
#include <IvyyyD3D.h>

BarnesHutCS::BarnesHutCS()
	: ComputeShader (L"BarnesHutCS.hlsl")
{
}

void BarnesHutCS::ShutdownShader()
{
	ComputeShader::ShutdownShader();

	if (m_inputBuffer)
	{
		m_inputBuffer->Release();
		m_inputBuffer = nullptr;
	}

	if (m_inputBufferSrv)
	{
		m_inputBufferSrv->Release();
		m_inputBufferSrv = nullptr;
	}

	if (m_outputBuffer)
	{
		m_outputBuffer->Release();
		m_outputBuffer = nullptr;
	}

	if (m_outputResultBuffer)
	{
		m_outputResultBuffer->Release();
		m_outputResultBuffer = nullptr;
	}

	if (m_outputBufferUav)
	{
		m_outputBufferUav->Release();
		m_outputBufferUav = nullptr;
	}
}

void BarnesHutCS::CopyData()
{
	if (IsDone())
	{
		ID3D11DeviceContext* deviceContext = D3D::Instance().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(m_outputResultBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

		if (SUCCEEDED (hr))
		{
			int* data = reinterpret_cast <int*> (mappedResource.pData);
			int test = data[0];
			deviceContext->Unmap(m_outputResultBuffer, 0);
		}
	}
}

bool BarnesHutCS::BindShaderParameters (ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources (0, 1, &m_inputBufferSrv);
	deviceContext->CSSetUnorderedAccessViews (0, 1, &m_outputBufferUav, nullptr);
	return true;
}

void BarnesHutCS::UnbindShaderParameters(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	deviceContext->CSSetShaderResources(0, 1, &nullSRV);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	deviceContext->CopyResource(m_outputResultBuffer, m_outputBuffer);
}

bool BarnesHutCS::InitShaderParameters (ID3D11Device* device)
{
	//Input buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(int) * m_maxSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(int);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_inputBuffer);

	if (FAILED(result))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = 1;

	result = device->CreateShaderResourceView(m_inputBuffer, &srvDesc, &m_inputBufferSrv);

	if (FAILED(result))
		return false;

	//Output buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(int) * m_maxSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(int);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_outputBuffer);

	if (FAILED(result))
		return false;

	//Output result buffer
	bufferDesc.BindFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	result = device->CreateBuffer(&bufferDesc, NULL, &m_outputResultBuffer);

	if (FAILED(result))
		return false;

	//Output uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = m_maxSize;

	result = device->CreateUnorderedAccessView (m_outputBuffer, &uavDesc, &m_outputBufferUav);

	if (FAILED(result))
		return false;

	return true;
}
