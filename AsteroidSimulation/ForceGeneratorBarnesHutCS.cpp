#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"
#include "ShaderUtils.h"

ForceGeneratorBarnesHutCS::ForceGeneratorBarnesHutCS()
	: m_deviceContext (D3D::Instance().GetDeviceContext())
{
}

void ForceGeneratorBarnesHutCS::Start()
{
	InitShaderParameters();

	m_csMortonCodes.Initialize();
	m_csMortonCodes.m_mortonCodesBufferUav = m_uavMortonCodeBuffer;
	m_csMortonCodes.m_srvPositionBuffer = m_srvPositionBuffer;
	m_csMortonCodes.m_settingsBuffer = m_settingsBuffer;

	m_csUpfrontHistrogram.Initialize();
	m_csUpfrontHistrogram.m_srvMortonCodesBuffer = m_srvMortonCodeBuffer;
	m_csUpfrontHistrogram.m_uavGlobalHistogramBuffer = m_uavGlobalHistogramBuffer;
	m_csUpfrontHistrogram.m_settingsBuffer = m_settingsBuffer;

	m_csExclusiveSum.Initialize();
	m_csExclusiveSum.m_srvGlobalHistogramBuffer = m_srvGlobalHistogramBuffer;
	m_csExclusiveSum.m_uavExclusivePrefixSumBuffer = m_uavExclusivePrefixSumBuffer;

	m_csDigitBinning.Initialize();
	m_csDigitBinning.m_mortonCodesBufferUav = m_uavMortonCodeBuffer;
	m_csDigitBinning.m_mortonCodesBufferSrv = m_srvMortonCodeBuffer;
	m_csDigitBinning.m_exclusivePrefixSumBufferSrv = m_srvEclusivePrefixSumBuffer;
	//m_csDigitBinning.m_uavMassBuffer = m_uavMassBuffer;
	//m_csDigitBinning.m_uavPositionBuffer = m_uavPositionBuffer;
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
	SetTestShaderParameter();
	//ToDo optimize dispatch count
	//m_csMortonCodes.Dispatch(m_objects.size(), 1, 1);

	//SM count for 3070 laptop is 40
	//We dispatch the kernel per SM core
	SetSettingParameter (MAX_CAPACITY, 40);
	m_csUpfrontHistrogram.Dispatch(40, 1, 1);

	SetSettingParameter(MAX_CAPACITY, 4);
	m_csExclusiveSum.Dispatch(4,1,1);

	//m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_outMortonCodeBuffer);

	m_csDigitBinning.Dispatch(1,1,1);

	m_deviceContext->CopyResource(m_outputBuffer, m_exclusivePrefixSumBuffer);
	//m_computeShader.SetShaderParameters(m_objects);
	//m_computeShader.Dispatch(m_objects.size(), 1, 1);
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_csDigitBinning.IsDone())
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

	HRESULT result = m_deviceContext->Map(m_upPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_upMassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMass);

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
	m_deviceContext->Unmap(m_upPositionBuffer, 0);
	m_deviceContext->Unmap(m_upMassBuffer, 0);

	m_deviceContext->CopyResource(m_inPositionBuffer, m_upPositionBuffer);
	m_deviceContext->CopyResource(m_inMassBuffer, m_upMassBuffer);

	m_csMortonCodes.SetShaderParameter (m_boundsMin, m_boundsMax);

	//Clear UAV
	UINT clearValues[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearUnorderedAccessViewUint (m_uavMortonCodeBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint (m_uavPositionBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint (m_uavMassBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint (m_uavGlobalHistogramBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint (m_uavExclusivePrefixSumBuffer, clearValues);
}

void ForceGeneratorBarnesHutCS::SetTestShaderParameter()
{
	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedMortonCode;
	D3D11_MAPPED_SUBRESOURCE mappedPosition;
	D3D11_MAPPED_SUBRESOURCE mappedMass;

	HRESULT result = m_deviceContext->Map(m_upMortonCodeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMortonCode);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_upPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_upMassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMass);

	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	uint32_t* mortonPtr = (uint32_t*)mappedMortonCode.pData;
	XMFLOAT3* positionPtr = (XMFLOAT3*)mappedPosition.pData;
	float* massPtr = (float*)mappedMass.pData;

	for (int i = 0; i < MAX_CAPACITY; ++i)
	{
		Vector3 pos = Vector3::Zero;
		positionPtr[i].x = pos.x;
		positionPtr[i].y = pos.y;
		positionPtr[i].z = pos.z;
		massPtr[i] = 1.f;
		mortonPtr[i] = i;

		m_boundsMax.x = max(m_boundsMax.x, pos.x);
		m_boundsMax.y = max(m_boundsMax.y, pos.y);
		m_boundsMax.z = max(m_boundsMax.z, pos.z);

		m_boundsMin.x = min(m_boundsMin.x, pos.x);
		m_boundsMin.y = min(m_boundsMin.y, pos.y);
		m_boundsMin.z = min(m_boundsMin.z, pos.z);
	}

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_upMortonCodeBuffer, 0);
	m_deviceContext->Unmap(m_upPositionBuffer, 0);
	m_deviceContext->Unmap(m_upMassBuffer, 0);

	m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_upMortonCodeBuffer);
	m_deviceContext->CopyResource(m_inPositionBuffer, m_upPositionBuffer);
	m_deviceContext->CopyResource(m_inMassBuffer, m_upMassBuffer);

	m_csMortonCodes.SetShaderParameter(m_boundsMin, m_boundsMax);

	//Clear UAV
	UINT clearValues[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavMortonCodeBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavPositionBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavMassBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavGlobalHistogramBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavExclusivePrefixSumBuffer, clearValues);
}

void ForceGeneratorBarnesHutCS::SetSettingParameter(uint32_t elementCount, uint32_t dispatchGroupCount)
{
	//Settings buffer
	D3D11_MAPPED_SUBRESOURCE mappedSettings;

	HRESULT result = m_deviceContext->Map(m_settingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSettings);

	if (FAILED(result))
		return;

	CSSettings* settings = (CSSettings*)mappedSettings.pData;
	settings->elementCount = elementCount;
	settings->dispatchGroupCount = dispatchGroupCount;

	m_deviceContext->Unmap(m_settingsBuffer, 0);
}


HRESULT ForceGeneratorBarnesHutCS::InitMortonCodeShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
	&& ShaderUtils::CreateResourceBuffer (device, result, &m_inMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
	&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
	&& ShaderUtils::CreateUAVBuffer(device, result, m_outMortonCodeBuffer, &m_uavMortonCodeBuffer, MAX_CAPACITY)
	&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMortonCodeBuffer, &m_srvMortonCodeBuffer, MAX_CAPACITY);

	return result;
}

HRESULT ForceGeneratorBarnesHutCS::InitPositionShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outPositionBuffer, &m_uavPositionBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inPositionBuffer, &m_srvPositionBuffer, MAX_CAPACITY);

	return result;
}

HRESULT ForceGeneratorBarnesHutCS::InitMassShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outMassBuffer, &m_uavMassBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMassBuffer, &m_srvMassBuffer, MAX_CAPACITY);

	return result;
}

HRESULT ForceGeneratorBarnesHutCS::InitSortingShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_globalHistogramBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_globalHistogramBuffer, &m_uavGlobalHistogramBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_globalHistogramBuffer, &m_srvGlobalHistogramBuffer, MAX_CAPACITY)
		&&ShaderUtils::CreateResourceBuffer(device, result, &m_exclusivePrefixSumBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_exclusivePrefixSumBuffer, &m_uavExclusivePrefixSumBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_exclusivePrefixSumBuffer, &m_srvEclusivePrefixSumBuffer, MAX_CAPACITY);

	return result;
}

bool ForceGeneratorBarnesHutCS::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	HRESULT result;

	//Debug buffer
	ShaderUtils::CreateStagingBuffer (device, result, &m_outputBuffer, sizeof(uint32_t), MAX_CAPACITY);

	if (FAILED(result))
		return false;

	//Particle buffer
	result = InitMortonCodeShaderParameters(device);

	if (FAILED(result))
		return false;

	result = InitPositionShaderParameters(device);

	if (FAILED(result))
		return false;

	result = InitMassShaderParameters(device);

	if (FAILED(result))
		return false;

	//Sorting buffers
	result = InitSortingShaderParameters (device);

	if (FAILED(result))
		return false;

	//Settings buffer
	ShaderUtils::CreateConstBuffer (device, result, &m_settingsBuffer, sizeof(CSSettings));

	if (FAILED(result))
		return false;

	return true;
}

void ForceGeneratorBarnesHutCS::ReleaseShaderParameters()
{
	ShaderUtils::SafeRelease (&m_outputBuffer);
	ShaderUtils::SafeRelease (&m_settingsBuffer);

	ShaderUtils::SafeRelease (&m_upPositionBuffer);
	ShaderUtils::SafeRelease (&m_inPositionBuffer);
	ShaderUtils::SafeRelease (&m_outPositionBuffer);
	ShaderUtils::SafeRelease (&m_srvPositionBuffer);
	ShaderUtils::SafeRelease (&m_uavPositionBuffer);

	ShaderUtils::SafeRelease(&m_upMassBuffer);
	ShaderUtils::SafeRelease(&m_inMassBuffer);
	ShaderUtils::SafeRelease(&m_outMassBuffer);
	ShaderUtils::SafeRelease(&m_srvMassBuffer);
	ShaderUtils::SafeRelease(&m_uavMassBuffer);

	ShaderUtils::SafeRelease (&m_upMortonCodeBuffer);
	ShaderUtils::SafeRelease (&m_inMortonCodeBuffer);
	ShaderUtils::SafeRelease(&m_outMortonCodeBuffer);
	ShaderUtils::SafeRelease (&m_srvMortonCodeBuffer);
	ShaderUtils::SafeRelease (&m_uavMortonCodeBuffer);

	ShaderUtils::SafeRelease(&m_globalHistogramBuffer);
	ShaderUtils::SafeRelease (&m_uavGlobalHistogramBuffer);
	ShaderUtils::SafeRelease (&m_srvGlobalHistogramBuffer);

	ShaderUtils::SafeRelease(&m_exclusivePrefixSumBuffer);
	ShaderUtils::SafeRelease(&m_uavExclusivePrefixSumBuffer);
	ShaderUtils::SafeRelease (&m_srvEclusivePrefixSumBuffer);
}

void ForceGeneratorBarnesHutCS::CopyTestData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_outputBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		uint32_t data [MaxCapacity];

		memcpy (&data[0], mappedResource.pData, sizeof(uint32_t) * MaxCapacity);

		m_deviceContext->Unmap(m_outputBuffer, 0);
	}
}

