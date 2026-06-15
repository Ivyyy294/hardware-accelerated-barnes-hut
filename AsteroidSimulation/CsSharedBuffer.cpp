#include "pch.h"
#include "CsSharedBuffer.h"
#include "ShaderUtils.h"


CsSharedBuffer::CsSharedBuffer(uint32_t maxParticleCount)
	: m_deviceContext (D3D::Instance().GetDeviceContext())
	, m_maxParticleCount (maxParticleCount)
	, m_maxCapacity (maxParticleCount+ maxParticleCount)
{
}

HRESULT CsSharedBuffer::InitDebugShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateStagingBuffer(device, result, &m_outputBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_debugBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_debugBuffer, &m_uavDebugBuffer, 1024)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_dUint32Buffer, sizeof(uint32_t), m_maxCapacity)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_dFloatBuffer, sizeof(float), m_maxCapacity)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_dFloat3Buffer, sizeof(XMFLOAT3), m_maxCapacity)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_dFloat4Buffer, sizeof(XMFLOAT4), m_maxCapacity);

	return result;
}

HRESULT CsSharedBuffer::InitMortonCodeShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMortonCodeBuffer, sizeof(uint32_t), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inMortonCodeBuffer, sizeof(uint32_t), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMortonCodeBuffer, sizeof(uint32_t), m_maxParticleCount)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outMortonCodeBuffer, &m_uavMortonCodeBuffer, m_maxParticleCount)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMortonCodeBuffer, &m_srvMortonCodeBuffer, m_maxParticleCount);

	return result;
}

HRESULT CsSharedBuffer::InitPositionShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upPositionBuffer, sizeof(XMFLOAT3), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outPositionBuffer, sizeof(XMFLOAT3), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inPositionBuffer, sizeof(XMFLOAT3), m_maxParticleCount)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outPositionBuffer, &m_uavPositionBuffer, m_maxParticleCount)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inPositionBuffer, &m_srvPositionBuffer, m_maxParticleCount);

	return result;
}

HRESULT CsSharedBuffer::InitIndexMappingShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_inNewToOldIndexBuffer, sizeof(uint32_t), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outNewToOldIndexBuffer, sizeof(uint32_t), m_maxParticleCount)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outNewToOldIndexBuffer, &m_uavNewToOldIndexBuffer, m_maxParticleCount)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inNewToOldIndexBuffer, &m_srvNewToOldIndexBuffer, m_maxParticleCount);

	return result;
}

HRESULT CsSharedBuffer::InitMassShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMassBuffer, sizeof(float), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMassBuffer, sizeof(float), m_maxParticleCount)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inMassBuffer, sizeof(float), m_maxParticleCount)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outMassBuffer, &m_uavMassBuffer, m_maxParticleCount)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMassBuffer, &m_srvMassBuffer, m_maxParticleCount);

	return result;
}

HRESULT CsSharedBuffer::InitSortingShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = m_globalHistogram.Init (device, result, sizeof(uint32_t), 1024)
		&& m_exclusivePrefixSum.Init(device, result, sizeof(uint32_t), 1024);

	return result;
}

HRESULT CsSharedBuffer::InitCornerStoneShaderParameters(ID3D11Device* device)
{
	//L = 10
	uint32_t maxK = m_maxCapacity;

	HRESULT result;

	uint32_t* k = new uint32_t[m_maxCapacity]{0, 1 << 3 * 10};
	D3D11_SUBRESOURCE_DATA initalK = {};
	initalK.pSysMem = k;

	uint32_t counter[3] = { 1, 0, 0};
	D3D11_SUBRESOURCE_DATA initalCount = {};
	initalCount.pSysMem = counter;

		// K Buffer
	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_inKBuffer, sizeof(uint32_t), maxK, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, &initalK)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inKBuffer, &m_srvKBuffer, maxK)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outKBuffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outKBuffer, &m_uavKBuffer, maxK)
		// N Buffer
		&& m_n.Init(device, result, sizeof(uint32_t), m_maxParticleCount)
		// O Buffer
		&& m_o1.Init(device, result, sizeof(uint32_t), maxK)
		&& m_o2.Init(device, result, sizeof(uint32_t), maxK)
		//Counter Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_counterBuffer, sizeof(uint32_t), 3, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, &initalCount)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_stagingCounterBuffer, sizeof(uint32_t), 3)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_counterBuffer, &m_srvCounterBuffer, 3)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_counterBuffer, &m_uavCounterBuffer, 3)
		//NK Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inNkBuffer, sizeof(uint32_t), m_maxCapacity)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outNkBuffer, sizeof(uint32_t), m_maxCapacity)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inNkBuffer, &m_srvNkBuffer, m_maxCapacity)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outNkBuffer, &m_uavNkBuffer, m_maxCapacity)
		//LO Buffer
		&& m_lo.Init(device, result, sizeof(uint32_t), 12)
		//CO Buffer
		&& m_co.Init(device, result, sizeof(uint32_t), m_maxCapacity)
		//LI Buffer
		&& m_li.Init(device, result, sizeof(uint32_t), m_maxCapacity)
		//LC Buffer
		&& m_lc.Init(device, result, sizeof(uint32_t), m_maxCapacity);

	return result;
}

HRESULT CsSharedBuffer::InitBarnesHutShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateConstBuffer(device, result, &m_bhSettingsBuffer, sizeof(BhSettings))
		//Cell Multipole Data
		&& m_cellMass.Init(device, result, sizeof(float), m_maxCapacity, 0, DXGI_FORMAT_R32_FLOAT)
		&& m_cellCom.Init(device, result, sizeof(XMFLOAT3), m_maxCapacity)
		//Force
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_forceBuffer, sizeof(XMFLOAT3), m_maxParticleCount)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_outForceBuffer, sizeof(XMFLOAT3), m_maxParticleCount)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_forceBuffer, &m_uavForceBuffer, m_maxParticleCount)
		//Cell Data
		&& m_cellGeoData.Init(device, result, sizeof(XMFLOAT4), m_maxCapacity, 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	return result;
}

void CsSharedBuffer::RecordBufferSnapshotXMFLOAT4(ID3D11Buffer* dest, ID3D11Buffer* source, const string name, const size_t count, int valIndex)
{
	//m_deviceContext->CopyResource (dest, source);
	m_deviceContext->CopySubresourceRegion(dest, 0, 0, 0, 0, source, 0, nullptr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(dest, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		XMFLOAT4* data = (XMFLOAT4*)mappedResource.pData;

		string memDump = name + ";";
		memDump.reserve(sizeof(XMFLOAT4) * count);

		for (size_t i = 0; i < count; ++i)
		{
			if (valIndex == 0)
				memDump += std::to_string(data[i].x) + ";";
			else if (valIndex == 1)
				memDump += std::to_string(data[i].y) + ";";
			else if (valIndex == 2)
				memDump += std::to_string(data[i].z) + ";";
			else
				memDump += std::to_string(data[i].w) + ";";
		}

		Session::Me().m_gpuMemDumpFile.WriteLine(memDump);

		m_deviceContext->Unmap(dest, 0);
	}
}


bool CsSharedBuffer::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	HRESULT result = InitDebugShaderParameters (device);

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

	result = InitIndexMappingShaderParameters(device);

	if (FAILED(result))
		return false;

	//Sorting buffers
	result = InitSortingShaderParameters(device);

	if (FAILED(result))
		return false;

	//Settings buffer
	ShaderUtils::CreateConstBuffer(device, result, &m_settingsBuffer, sizeof(CsSettings));

	if (FAILED(result))
		return false;

	result = InitCornerStoneShaderParameters(device);

	if (FAILED(result))
		return false;

	result = InitBarnesHutShaderParameters(device);

	if (FAILED(result))
		return false;

	return true;
}

void CsSharedBuffer::ReleaseShaderParameters()
{
	ShaderUtils::SafeRelease(&m_debugBuffer);
	ShaderUtils::SafeRelease(&m_dUint32Buffer);
	ShaderUtils::SafeRelease(&m_dFloatBuffer);
	ShaderUtils::SafeRelease(&m_dFloat3Buffer);
	ShaderUtils::SafeRelease(&m_dFloat4Buffer);

	ShaderUtils::SafeRelease(&m_outputBuffer);
	ShaderUtils::SafeRelease(&m_settingsBuffer);

	ShaderUtils::SafeRelease(&m_upPositionBuffer);
	ShaderUtils::SafeRelease(&m_inPositionBuffer);
	ShaderUtils::SafeRelease(&m_outPositionBuffer);
	ShaderUtils::SafeRelease(&m_srvPositionBuffer);
	ShaderUtils::SafeRelease(&m_uavPositionBuffer);

	ShaderUtils::SafeRelease(&m_upMassBuffer);
	ShaderUtils::SafeRelease(&m_inMassBuffer);
	ShaderUtils::SafeRelease(&m_outMassBuffer);
	ShaderUtils::SafeRelease(&m_srvMassBuffer);
	ShaderUtils::SafeRelease(&m_uavMassBuffer);

	ShaderUtils::SafeRelease(&m_upMortonCodeBuffer);
	ShaderUtils::SafeRelease(&m_inMortonCodeBuffer);
	ShaderUtils::SafeRelease(&m_outMortonCodeBuffer);
	ShaderUtils::SafeRelease(&m_srvMortonCodeBuffer);
	ShaderUtils::SafeRelease(&m_uavMortonCodeBuffer);

	m_globalHistogram.Release();
	m_exclusivePrefixSum.Release();

	ShaderUtils::SafeRelease(&m_inKBuffer);
	ShaderUtils::SafeRelease(&m_outKBuffer);
	ShaderUtils::SafeRelease(&m_srvKBuffer);
	ShaderUtils::SafeRelease(&m_uavKBuffer);

	m_o1.Release();
	m_o2.Release();

	ShaderUtils::SafeRelease(&m_counterBuffer);
	ShaderUtils::SafeRelease(&m_stagingCounterBuffer);
	ShaderUtils::SafeRelease(&m_uavCounterBuffer);
	ShaderUtils::SafeRelease(&m_srvCounterBuffer);

	ShaderUtils::SafeRelease(&m_inNkBuffer);
	ShaderUtils::SafeRelease(&m_outNkBuffer);
	ShaderUtils::SafeRelease(&m_uavNkBuffer);
	ShaderUtils::SafeRelease(&m_srvNkBuffer);

	m_lo.Release();
	m_co.Release();

	m_cellMass.Release();
	m_cellCom.Release();
	m_cellGeoData.Release();

	m_li.Release();
	m_lo.Release();
}


void CsSharedBuffer::SetSettingParameter(uint32_t elementCount, uint32_t dispatchGroupCount, uint32_t digitPlace)
{
	//Settings buffer
	D3D11_MAPPED_SUBRESOURCE mappedSettings;

	HRESULT result = m_deviceContext->Map(m_settingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSettings);

	if (FAILED(result))
		return;

	CsSettings* settings = (CsSettings*)mappedSettings.pData;
	settings->elementCount = elementCount;
	settings->dispatchGroupCount = dispatchGroupCount;
	settings->digitPlace = digitPlace;

	m_deviceContext->Unmap(m_settingsBuffer, 0);
}

void CsSharedBuffer::SetBhSettingsParameter(uint32_t elementCount, float cellWidth, float ugc, float theta)
{
	//Settings buffer
	D3D11_MAPPED_SUBRESOURCE mappedSettings;

	HRESULT result = m_deviceContext->Map(m_bhSettingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSettings);

	if (FAILED(result))
		return;

	BhSettings* settings = (BhSettings*)mappedSettings.pData;
	settings->pCount = elementCount;
	settings->globalCellWidth = cellWidth;
	settings->ugc = ugc;
	settings->theta = theta;

	m_deviceContext->Unmap(m_bhSettingsBuffer, 0);
}

void CsSharedBuffer::ClearUavBuffer()
{
	//Clear UAV
	UINT clearValues[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavDebugBuffer, clearValues);

	FLOAT clearValuesF[4] = { 0.f, 0.f, 0.f, 0.f };
	m_deviceContext->ClearUnorderedAccessViewFloat(m_cellGeoData.m_uav, clearValuesF);

	FLOAT clearValuesF2[4] = { -1.f, -1.f, -1.f, -1.f };
	m_deviceContext->ClearUnorderedAccessViewFloat(m_cellMass.m_uav, clearValuesF2);
}

void CsSharedBuffer::SwapParticleBuffer()
{
	m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_outMortonCodeBuffer);
	m_deviceContext->CopyResource(m_inPositionBuffer, m_outPositionBuffer);
	m_deviceContext->CopyResource(m_inMassBuffer, m_outMassBuffer);
	m_deviceContext->CopyResource(m_inNewToOldIndexBuffer, m_outNewToOldIndexBuffer);
}

void CsSharedBuffer::RecordBufferSnapshot()
{
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_outMortonCodeBuffer, "M", m_maxParticleCount);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_outKBuffer, "K", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_n.m_buffer, "N", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_o1.m_buffer, "O1", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_o2.m_buffer, "O2", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_outNkBuffer, "NK", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_lo.m_buffer, "LO", 12);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_co.m_buffer, "CO", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_li.m_buffer, "LI", m_maxCapacity);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_lc.m_buffer, "LC", m_maxCapacity);
	RecordBufferSnapshot <float> (m_dFloatBuffer, m_cellMass.m_buffer, "CM", m_maxCapacity);
	RecordBufferSnapshotXMFLOAT4 (m_dFloat4Buffer, m_cellGeoData.m_buffer, "CW", m_maxCapacity, 3);
	RecordBufferSnapshot <uint32_t> (m_dUint32Buffer, m_counterBuffer, "n", 2);

	//m_deviceContext->CopyResource (dest, source);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_bhSettingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		BhSettings* data = (BhSettings*)mappedResource.pData;

		string memDump = "CBUFFER;";
		memDump += std::to_string(data->pCount) + ";";
		memDump += std::to_string(data->globalCellWidth) + ";";
		memDump += std::to_string(data->ugc) + ";";
		memDump += std::to_string(data->theta) + ";";

		Session::Me().m_gpuMemDumpFile.WriteLine(memDump);

		m_deviceContext->Unmap(m_bhSettingsBuffer, 0);
	}

	Session::Me().m_gpuMemDumpFile.WriteLine("");
}
