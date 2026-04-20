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

	m_computeShader.SetMaxSize (m_objects.size());
	m_computeShader.Initialize ();
}

void ForceGeneratorBarnesHutCS::Exit()
{
	ReleaseShaderParameters();
	m_computeShader.ShutdownShader();
}

void ForceGeneratorBarnesHutCS::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void ForceGeneratorBarnesHutCS::Dispatch()
{
	SetShaderParameter();
	m_csMortonCodes.Dispatch(m_objects.size(), 1, 1);

	//m_computeShader.SetShaderParameters(m_objects);
	//m_computeShader.Dispatch(m_objects.size(), 1, 1);
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_csMortonCodes.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	//m_csMortonCodes.CopyData();
}

void ForceGeneratorBarnesHutCS::SetShaderParameter()
{
	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedPosition;

	HRESULT result = m_deviceContext->Map(m_positionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	XMFLOAT3* positionPtr = (XMFLOAT3*)mappedPosition.pData;

	XMFLOAT3 boundingBoxMax{ 0.f, 0.f, 0.f };
	XMFLOAT3 boundingBoxMin{ 0.f, 0.f, 0.f };

	for (int i = 0; i < m_objects.size() && i < MaxCapacity; ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		positionPtr[i].x = pos.x;
		positionPtr[i].y = pos.y;
		positionPtr[i].z = pos.z;

		boundingBoxMax.x = max(boundingBoxMax.x, pos.x);
		boundingBoxMax.y = max(boundingBoxMax.y, pos.y);
		boundingBoxMax.z = max(boundingBoxMax.z, pos.z);

		boundingBoxMin.x = min(boundingBoxMin.x, pos.x);
		boundingBoxMin.y = min(boundingBoxMin.y, pos.y);
		boundingBoxMin.z = min(boundingBoxMin.z, pos.z);
	}

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_positionBuffer, 0);

	m_csMortonCodes.SetShaderParameter (m_boundsMin, m_boundsMax);
}


bool ForceGeneratorBarnesHutCS::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	//Position buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(XMFLOAT3);

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &m_positionBuffer);

	if (FAILED(result))
		return false;

	//Position buffer uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	result = device->CreateUnorderedAccessView(m_positionBuffer, &uavDesc, &m_positionBufferUav);

	if (FAILED(result))
		return false;

	//Morton Codes buffer
	bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(size_t) * MaxCapacity;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//ToD0: set CPUAccessFlags to 0
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(size_t);

	result = device->CreateBuffer(&bufferDesc, NULL, &m_positionBuffer);

	if (FAILED(result))
		return false;

	//Morton Codes buffer uav
	uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxCapacity;

	result = device->CreateUnorderedAccessView(m_positionBuffer, &uavDesc, &m_positionBufferUav);

	if (FAILED(result))
		return false;

	return true;
}

void ForceGeneratorBarnesHutCS::ReleaseShaderParameters()
{
	if (m_positionBuffer)
	{
		m_positionBuffer->Release();
		m_positionBuffer = nullptr;
	}

	if (m_positionBufferUav)
	{
		m_positionBufferUav->Release();
		m_positionBufferUav = nullptr;
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
}



