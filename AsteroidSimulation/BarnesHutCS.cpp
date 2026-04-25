#include "pch.h"
#include "BarnesHutCS.h"
#include <IvyyyD3D.h>

BarnesHutCS::BarnesHutCS()
	: ComputeShader (L"BarnesHutCS.hlsl")
{
}

void BarnesHutCS::CopyData()
{
	if (IsDone())
	{
		ID3D11DeviceContext* m_deviceContext = D3D::Instance().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = m_deviceContext->Map(m_outputResultBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

		if (SUCCEEDED (hr))
		{
			int* data = reinterpret_cast <int*> (mappedResource.pData);
			int test = data[0];
			m_deviceContext->Unmap(m_outputResultBuffer, 0);
		}
	}
}

void BarnesHutCS::SetShaderParameters(const std::vector<PhysicObject*>& objects)
{
	ID3D11DeviceContext* m_deviceContext = D3D::Instance().GetDeviceContext();
	
	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedPosition;
	D3D11_MAPPED_SUBRESOURCE mappedMass;
	D3D11_MAPPED_SUBRESOURCE mappedUnitCube;

	HRESULT result = m_deviceContext->Map(m_positionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_inMassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMass);

	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	XMFLOAT3* positionPtr = (XMFLOAT3*)mappedPosition.pData;
	float* massPtr = (float*)mappedMass.pData;

	XMFLOAT3 boundingBoxMax {0.f, 0.f, 0.f};
	XMFLOAT3 boundingBoxMin { 0.f, 0.f, 0.f };

	for (int i = 0; i < objects.size() && i < m_maxSize; ++i)
	{
		Vector3 pos = objects[i]->GetTransform()->GetPosition();
		positionPtr[i].x = pos.x;
		positionPtr[i].y = pos.y;
		positionPtr[i].z = pos.z;

		massPtr[i] = objects[i]->GetMass();

		boundingBoxMax.x = max(boundingBoxMax.x, pos.x);
		boundingBoxMax.y = max(boundingBoxMax.y, pos.y);
		boundingBoxMax.z = max(boundingBoxMax.z, pos.z);

		boundingBoxMin.x = min(boundingBoxMin.x, pos.x);
		boundingBoxMin.y = min(boundingBoxMin.y, pos.y);
		boundingBoxMin.z = min(boundingBoxMin.z, pos.z);
	}

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_positionBuffer, 0);
	m_deviceContext->Unmap(m_inMassBuffer, 0);

	result = m_deviceContext->Map(m_unitCubeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedUnitCube);

	if (FAILED(result))
		return;

	UnitCube* unitCube = (UnitCube*)mappedUnitCube.pData;
	unitCube->boundsMax = boundingBoxMax;
	unitCube->boundsMin = boundingBoxMin;

	m_deviceContext->Unmap(m_unitCubeBuffer, 0);
}

void BarnesHutCS::BindShaderParameters (ID3D11DeviceContext* m_deviceContext)
{
	m_deviceContext->CSSetConstantBuffers (0, 1, &m_unitCubeBuffer);
	m_deviceContext->CSSetShaderResources (0, 1, &m_positionBufferSrv);
	m_deviceContext->CSSetShaderResources (1, 1, &m_massBufferSrv);
	m_deviceContext->CSSetUnorderedAccessViews (0, 1, &m_outputBufferUav, nullptr);
}

void BarnesHutCS::UnbindShaderParameters(ID3D11DeviceContext* m_deviceContext)
{
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	m_deviceContext->CSSetConstantBuffers(0, 1, &nullBuffer);

	m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);

	m_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	m_deviceContext->CopyResource(m_outputResultBuffer, m_outputBuffer);
}

bool BarnesHutCS::InitShaderParameters (ID3D11Device* device)
{
	//position buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * m_maxSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(XMFLOAT3);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_positionBuffer);

	if (FAILED(result))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = 1;

	result = device->CreateShaderResourceView(m_positionBuffer, &srvDesc, &m_positionBufferSrv);

	if (FAILED(result))
		return false;

	//mass buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(float) * m_maxSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(float);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&bufferDesc, NULL, &m_inMassBuffer);

	if (FAILED(result))
		return false;

	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = 1;

	result = device->CreateShaderResourceView(m_inMassBuffer, &srvDesc, &m_massBufferSrv);

	if (FAILED(result))
		return false;

	//UnitCube buffer
	bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(UnitCube);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = device->CreateBuffer(&bufferDesc, NULL, &m_unitCubeBuffer);

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

void BarnesHutCS::ReleaseShaderParameters()
{
	if (m_positionBuffer)
	{
		m_positionBuffer->Release();
		m_positionBuffer = nullptr;
	}

	if (m_positionBufferSrv)
	{
		m_positionBufferSrv->Release();
		m_positionBufferSrv = nullptr;
	}

	if (m_unitCubeBuffer)
	{
		m_unitCubeBuffer->Release();
		m_unitCubeBuffer = nullptr;
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
