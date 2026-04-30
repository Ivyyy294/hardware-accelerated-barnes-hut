#include "pch.h"
#include "CsSharedBuffer.h"
#include "ShaderUtils.h"


CsSharedBuffer::CsSharedBuffer()
	: m_deviceContext (D3D::Instance().GetDeviceContext())
{
}

HRESULT CsSharedBuffer::InitMortonCodeShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMortonCodeBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outMortonCodeBuffer, &m_uavMortonCodeBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMortonCodeBuffer, &m_srvMortonCodeBuffer, MAX_CAPACITY);

	return result;
}

HRESULT CsSharedBuffer::InitPositionShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inPositionBuffer, sizeof(XMFLOAT3), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outPositionBuffer, &m_uavPositionBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inPositionBuffer, &m_srvPositionBuffer, MAX_CAPACITY);

	return result;
}

HRESULT CsSharedBuffer::InitMassShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateDynamicBuffer(device, result, &m_upMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inMassBuffer, sizeof(float), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outMassBuffer, &m_uavMassBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inMassBuffer, &m_srvMassBuffer, MAX_CAPACITY);

	return result;
}

HRESULT CsSharedBuffer::InitSortingShaderParameters(ID3D11Device* device)
{
	HRESULT result;

	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_globalHistogramBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_globalHistogramBuffer, &m_uavGlobalHistogramBuffer, 1024)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_globalHistogramBuffer, &m_srvGlobalHistogramBuffer, 1024)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_exclusivePrefixSumBuffer, sizeof(uint32_t), 1024)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_exclusivePrefixSumBuffer, &m_uavExclusivePrefixSumBuffer, 1024)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_exclusivePrefixSumBuffer, &m_srvEclusivePrefixSumBuffer, 1024);

	return result;
}

HRESULT CsSharedBuffer::InitCornerStoneShaderParameters(ID3D11Device* device)
{
	//L = 10
	uint32_t maxK = MAX_CAPACITY + 1;

	HRESULT result;

	uint32_t k [MAX_CAPACITY + 1] = {0, 1 << 3 * 10};
	D3D11_SUBRESOURCE_DATA initalK = {};
	initalK.pSysMem = k;

	uint32_t counter[2] = { 1, 0 };
	D3D11_SUBRESOURCE_DATA initalCount = {};
	initalCount.pSysMem = counter;

		// K Buffer
	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_inKBuffer, sizeof(uint32_t), maxK, &initalK)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inKBuffer, &m_srvKBuffer, maxK)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outKBuffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outKBuffer, &m_uavKBuffer, maxK)
		// N Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_nBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_nBuffer, &m_srvNBuffer, MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_nBuffer, &m_uavNBuffer, MAX_CAPACITY)
		// O Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_o1Buffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_o1Buffer, &m_srvO1Buffer, maxK)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_o1Buffer, &m_uavO1Buffer, maxK)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_o2Buffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_o2Buffer, &m_srvO2Buffer, maxK)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_o2Buffer, &m_uavO2Buffer, maxK)
		//Counter Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_counterBuffer, sizeof(uint32_t), 2, &initalCount)
		&& ShaderUtils::CreateStagingBuffer(device, result, &m_stagingCounterBuffer, sizeof(uint32_t), 2)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_counterBuffer, &m_srvCounterBuffer, 2)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_counterBuffer, &m_uavCounterBuffer, 2)
		//NK Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_inNkBuffer, sizeof(uint32_t), MAX_CAPACITY + MAX_CAPACITY)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outNkBuffer, sizeof(uint32_t), MAX_CAPACITY + MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inNkBuffer, &m_srvNkBuffer, MAX_CAPACITY + MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outNkBuffer, &m_uavNkBuffer, MAX_CAPACITY + MAX_CAPACITY)
		//LO Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_loBuffer, sizeof(uint32_t), 12)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_loBuffer, &m_srvLoBuffer, 12)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_loBuffer, &m_uavLoBuffer, 12)
		//CO Buffer
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_coBuffer, sizeof(uint32_t), MAX_CAPACITY + MAX_CAPACITY)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_coBuffer, &m_srvCoBuffer, MAX_CAPACITY + MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_coBuffer, &m_uavCoBuffer, MAX_CAPACITY + MAX_CAPACITY);

	return result;
}


bool CsSharedBuffer::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	HRESULT result;

	//Debug buffer
	ShaderUtils::CreateStagingBuffer(device, result, &m_outputBuffer, sizeof(uint32_t), 1024);
	ShaderUtils::CreateResourceBuffer(device, result, &m_debugBuffer, sizeof(uint32_t), 1024);
	ShaderUtils::CreateUAVBuffer(device, result, m_debugBuffer, &m_uavDebugBuffer, 1024);

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
	result = InitSortingShaderParameters(device);

	if (FAILED(result))
		return false;

	//Settings buffer
	ShaderUtils::CreateConstBuffer(device, result, &m_settingsBuffer, sizeof(CsSettings));

	if (FAILED(result))
		return false;

	InitCornerStoneShaderParameters(device);

	return true;
}

void CsSharedBuffer::ReleaseShaderParameters()
{
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

	ShaderUtils::SafeRelease(&m_globalHistogramBuffer);
	ShaderUtils::SafeRelease(&m_uavGlobalHistogramBuffer);
	ShaderUtils::SafeRelease(&m_srvGlobalHistogramBuffer);

	ShaderUtils::SafeRelease(&m_exclusivePrefixSumBuffer);
	ShaderUtils::SafeRelease(&m_uavExclusivePrefixSumBuffer);
	ShaderUtils::SafeRelease(&m_srvEclusivePrefixSumBuffer);

	ShaderUtils::SafeRelease(&m_inKBuffer);
	ShaderUtils::SafeRelease(&m_outKBuffer);
	ShaderUtils::SafeRelease(&m_srvKBuffer);
	ShaderUtils::SafeRelease(&m_uavKBuffer);

	ShaderUtils::SafeRelease(&m_o1Buffer);
	ShaderUtils::SafeRelease(&m_o2Buffer);
	ShaderUtils::SafeRelease(&m_srvO1Buffer);
	ShaderUtils::SafeRelease(&m_uavO2Buffer);

	ShaderUtils::SafeRelease(&m_counterBuffer);
	ShaderUtils::SafeRelease(&m_stagingCounterBuffer);
	ShaderUtils::SafeRelease(&m_uavCounterBuffer);
	ShaderUtils::SafeRelease(&m_srvCounterBuffer);

	ShaderUtils::SafeRelease(&m_inNkBuffer);
	ShaderUtils::SafeRelease(&m_outNkBuffer);
	ShaderUtils::SafeRelease(&m_uavNkBuffer);
	ShaderUtils::SafeRelease(&m_srvNkBuffer);

	ShaderUtils::SafeRelease(&m_loBuffer);
	ShaderUtils::SafeRelease(&m_uavLoBuffer);
	ShaderUtils::SafeRelease(&m_srvLoBuffer);

	ShaderUtils::SafeRelease(&m_coBuffer);
	ShaderUtils::SafeRelease(&m_uavCoBuffer);
	ShaderUtils::SafeRelease(&m_srvCoBuffer);
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

void CsSharedBuffer::ClearUavBuffer()
{
	//Clear UAV
	UINT clearValues[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavMortonCodeBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavPositionBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavMassBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavGlobalHistogramBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavExclusivePrefixSumBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavNBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavO2Buffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavNkBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavLoBuffer, clearValues);
	m_deviceContext->ClearUnorderedAccessViewUint(m_uavCoBuffer, clearValues);
}

void CsSharedBuffer::SwapParticleBuffer()
{
	m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_outMortonCodeBuffer);
	m_deviceContext->CopyResource(m_inPositionBuffer, m_outPositionBuffer);
	m_deviceContext->CopyResource(m_inMassBuffer, m_outMassBuffer);
}
