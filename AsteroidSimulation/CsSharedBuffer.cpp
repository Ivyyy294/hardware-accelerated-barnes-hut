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

	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_inKBuffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_inKBuffer, &m_srvKBuffer, maxK)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_outKBuffer, sizeof(uint32_t), maxK)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_outKBuffer, &m_uavKBuffer, maxK)
		&& ShaderUtils::CreateResourceBuffer(device, result, &m_nBuffer, sizeof(uint32_t), MAX_CAPACITY)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_nBuffer, &m_uavNBuffer, MAX_CAPACITY);

	if (ok)
	{
		ID3D11DeviceContext* deviceContext = D3D::Instance().GetDeviceContext();
		uint32_t init[2] = { 0, MAX_CAPACITY };
		D3D11_BOX box = { 0,0,0, 2 * sizeof(uint32_t),1,1 };
		deviceContext->UpdateSubresource(m_inKBuffer, 0, &box, init, 0, 0);
	}

	return result;
}


bool CsSharedBuffer::InitShaderParameters()
{
	ID3D11Device* device = D3D::Instance().GetDevice();

	if (device == nullptr)
		return false;

	HRESULT result;

	//Debug buffer
	ShaderUtils::CreateStagingBuffer(device, result, &m_outputBuffer, sizeof(uint32_t), MAX_CAPACITY);

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
}

void CsSharedBuffer::SwapParticleBuffer()
{
	m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_outMortonCodeBuffer);
	m_deviceContext->CopyResource(m_inPositionBuffer, m_outPositionBuffer);
	m_deviceContext->CopyResource(m_inMassBuffer, m_outMassBuffer);
}
