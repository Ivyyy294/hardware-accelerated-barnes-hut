#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"

ForceGeneratorBarnesHutCS::ForceGeneratorBarnesHutCS()
	: m_deviceContext (D3D::Instance().GetDeviceContext())
{
}

void ForceGeneratorBarnesHutCS::Start()
{
	InitShaderParameters();

	m_csMortonCodes.Initialize();
	m_csMortonCodes.m_mortonCodesBufferUav = m_mortonCodeBufferUav;
	m_csMortonCodes.m_positionBufferUav = m_positionBufferUav;
	m_csMortonCodes.m_settingsBuffer = m_settingsBuffer;

	m_csUpfrontHistrogram.Initialize();
	m_csUpfrontHistrogram.m_mortonCodesBufferUav = m_mortonCodeBufferUav;
	m_csUpfrontHistrogram.m_globalHistogramBufferUav = m_globalHistogramBufferUav;
	m_csUpfrontHistrogram.m_settingsBuffer = m_settingsBuffer;

	m_csExclusiveSum.Initialize();
	m_csExclusiveSum.m_globalHistogramBufferUav = m_globalHistogramBufferUav;
	m_csExclusiveSum.m_exclusivePrefixSumBufferUav = m_exclusivePrefixSumBufferUav;

	m_csDigitBinning.Initialize();
	m_csDigitBinning.m_mortonCodesBufferUav = m_mortonCodeBufferUav;
	m_csDigitBinning.m_exclusivePrefixSumBufferUav = m_exclusivePrefixSumBufferUav;
	m_csDigitBinning.m_massBufferUav = m_massBufferUav;
	m_csDigitBinning.m_positionBufferUav = m_positionBufferUav;
}

void ForceGeneratorBarnesHutCS::Exit()
{
	ReleaseShaderParameters();
	m_computeShader.ShutdownShader();
	m_csUpfrontHistrogram.ShutdownShader();
}

void ForceGeneratorBarnesHutCS::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void ForceGeneratorBarnesHutCS::Dispatch()
{
	SetShaderParameter();
	m_csMortonCodes.Dispatch(m_objects.size(), 1, 1);

	//SM count for 3070 laptop is 40
	//We dispatch the kernel per SM core
	m_csUpfrontHistrogram.Dispatch(40, 1, 1);
	m_csExclusiveSum.Dispatch(4,1,1);

	m_deviceContext->CopyResource(m_outputBuffer, m_exclusivePrefixSumBuffer);
	//m_computeShader.SetShaderParameters(m_objects);
	//m_computeShader.Dispatch(m_objects.size(), 1, 1);
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_csUpfrontHistrogram.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	
	CopyTestData();
}

void ForceGeneratorBarnesHutCS::SetShaderParameter()
{
	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedPosition;
	D3D11_MAPPED_SUBRESOURCE mappedMass;

	HRESULT result = m_deviceContext->Map(m_positionUploadBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_massUploadBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMass);

	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	XMFLOAT3* positionPtr = (XMFLOAT3*)mappedPosition.pData;
	float * massPtr = (float*)mappedMass.pData;

	for (int i = 0; i < m_objects.size() && i < MaxCapacity; ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		positionPtr[i].x = pos.x;
		positionPtr[i].y = pos.y;
		positionPtr[i].z = pos.z;
		massPtr[i] = m_objects[i]->GetMass();

		m_boundsMax.x = max(m_boundsMax.x, pos.x);
		m_boundsMax.y = max(m_boundsMax.y, pos.y);
		m_boundsMax.z = max(m_boundsMax.z, pos.z);

		m_boundsMin.x = min(m_boundsMin.x, pos.x);
		m_boundsMin.y = min(m_boundsMin.y, pos.y);
		m_boundsMin.z = min(m_boundsMin.z, pos.z);
	}

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_positionUploadBuffer, 0);
	m_deviceContext->Unmap(m_massUploadBuffer, 0);

	m_deviceContext->CopyResource(m_positionBuffer, m_positionUploadBuffer);
	m_deviceContext->CopyResource(m_massBuffer, m_massUploadBuffer);

	m_csMortonCodes.SetShaderParameter (m_boundsMin, m_boundsMax);

	//Settings buffer
	D3D11_MAPPED_SUBRESOURCE mappedSettings;

	result = m_deviceContext->Map(m_settingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSettings);

	if (FAILED(result))
		return;

	CSSettings* settings = (CSSettings*)mappedSettings.pData;
	settings->elementCount = m_objects.size();

	m_deviceContext->Unmap (m_settingsBuffer, 0);

	//Clear UAV
	UINT clearValues[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearUnorderedAccessViewUint (m_globalHistogramBufferUav, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint (m_exclusivePrefixSumBufferUav, clearValues);
}


HRESULT ForceGeneratorBarnesHutCS::InitPositionShaderParameters(ID3D11Device* device)
{
	//Position upload buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(XMFLOAT3);

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_positionUploadBuffer);

	if (FAILED(result))
		return result;

	//Position buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(XMFLOAT3);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_positionBuffer);

	if (FAILED(result))
		return result;

	//Position buffer uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	return device->CreateUnorderedAccessView(m_positionBuffer, &uavDesc, &m_positionBufferUav);
}

HRESULT ForceGeneratorBarnesHutCS::InitMassShaderParameters(ID3D11Device* device)
{
	//Position upload buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(float) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(float);

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_massUploadBuffer);

	if (FAILED(result))
		return result;

	//Position buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.ByteWidth = sizeof(float) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(float);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_massBuffer);

	if (FAILED(result))
		return result;

	//Position buffer uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	return device->CreateUnorderedAccessView(m_massBuffer, &uavDesc, &m_massBufferUav);
}

HRESULT ForceGeneratorBarnesHutCS::InitSortingShaderParameters(ID3D11Device* device)
{
	//Global histogram buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_globalHistogramBuffer);

	if (FAILED(result))
		return result;

	//Global histogram buffer uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	result = device->CreateUnorderedAccessView(m_globalHistogramBuffer, &uavDesc, &m_globalHistogramBufferUav);

	if (FAILED(result))
		return result;

	//Exclusive prefix sum buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_exclusivePrefixSumBuffer);

	if (FAILED(result))
		return result;

	//##########Test buffer#############
	bufferDesc.BindFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	result = device->CreateBuffer(&bufferDesc, NULL, &m_outputBuffer);

	if (FAILED(result))
		return result;
	//##################################

	//Exclusive prefix sum buffer uav
	uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	result = device->CreateUnorderedAccessView(m_exclusivePrefixSumBuffer, &uavDesc, &m_exclusivePrefixSumBufferUav);

	if (FAILED(result))
		return result;
}

bool ForceGeneratorBarnesHutCS::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	//Particle buffer
	HRESULT result = InitPositionShaderParameters(device);

	if (FAILED(result))
		return false;

	result = InitMassShaderParameters(device);

	if (FAILED(result))
		return false;

	//Morton Codes buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_mortonCodeBuffer);

	if (FAILED(result))
		return false;

	//Morton Codes buffer uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	result = device->CreateUnorderedAccessView(m_mortonCodeBuffer, &uavDesc, &m_mortonCodeBufferUav);

	if (FAILED(result))
		return false;

	//Sorting buffers
	result = InitSortingShaderParameters (device);

	if (FAILED(result))
		return false;

	//Settings buffer
	bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(CSSettings);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = device->CreateBuffer(&bufferDesc, NULL, &m_settingsBuffer);

	if (FAILED(result))
		return false;

	return true;
}

void ForceGeneratorBarnesHutCS::ReleaseShaderParameters()
{
	if (m_outputBuffer)
	{
		m_outputBuffer->Release();
		m_outputBuffer = nullptr;
	}

	if (m_settingsBuffer)
	{
		m_settingsBuffer->Release();
		m_settingsBuffer = nullptr;
	}

	if (m_positionBuffer)
	{
		m_positionBuffer->Release();
		m_positionBuffer = nullptr;
	}

	if (m_positionUploadBuffer)
	{
		m_positionUploadBuffer->Release();
		m_positionUploadBuffer = nullptr;
	}

	if (m_positionBufferUav)
	{
		m_positionBufferUav->Release();
		m_positionBufferUav = nullptr;
	}

	if (m_massBuffer)
	{
		m_massBuffer->Release();
		m_massBuffer = nullptr;
	}

	if (m_massUploadBuffer)
	{
		m_massUploadBuffer->Release();
		m_massUploadBuffer = nullptr;
	}

	if (m_massBufferUav)
	{
		m_massBufferUav->Release();
		m_massBufferUav = nullptr;
	}

	if (m_mortonCodeBuffer)
	{
		m_mortonCodeBuffer->Release();
		m_mortonCodeBuffer = nullptr;
	}

	if (m_mortonCodeBufferUav)
	{
		m_mortonCodeBufferUav->Release();
		m_mortonCodeBufferUav = nullptr;
	}

	if (m_globalHistogramBuffer)
	{
		m_globalHistogramBuffer->Release();
		m_globalHistogramBuffer = nullptr;
	}

	if (m_globalHistogramBufferUav)
	{
		m_globalHistogramBufferUav->Release();
		m_globalHistogramBufferUav = nullptr;
	}

	if (m_exclusivePrefixSumBuffer)
	{
		m_exclusivePrefixSumBuffer->Release();
		m_exclusivePrefixSumBuffer = nullptr;
	}

	if (m_exclusivePrefixSumBufferUav)
	{
		m_exclusivePrefixSumBufferUav->Release();
		m_exclusivePrefixSumBufferUav = nullptr;
	}
}

void ForceGeneratorBarnesHutCS::CopyTestData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_outputBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		uint32_t data [1024];

		memcpy (&data[0], mappedResource.pData, sizeof(uint32_t) * 1024);

		m_deviceContext->Unmap(m_outputBuffer, 0);
	}
}

