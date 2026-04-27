#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"
#include "ShaderUtils.h"

ForceGeneratorBarnesHutCS::ForceGeneratorBarnesHutCS()
	: m_deviceContext (D3D::Instance().GetDeviceContext())
{
}

void ForceGeneratorBarnesHutCS::Start()
{
	m_buffer.InitShaderParameters();

	m_csMortonCodes.Initialize();
	m_csMortonCodes.m_uavMortonCodesBuffer = m_buffer.m_uavMortonCodeBuffer;
	m_csMortonCodes.m_srvPositionBuffer = m_buffer.m_srvPositionBuffer;
	m_csMortonCodes.m_settingsBuffer = m_buffer.m_settingsBuffer;

	m_csUpfrontHistrogram.Initialize();
	m_csUpfrontHistrogram.m_srvMortonCodesBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csUpfrontHistrogram.m_uavGlobalHistogramBuffer = m_buffer.m_uavGlobalHistogramBuffer;
	m_csUpfrontHistrogram.m_settingsBuffer = m_buffer.m_settingsBuffer;

	m_csExclusiveSum.Initialize();
	m_csExclusiveSum.m_srvGlobalHistogramBuffer = m_buffer.m_srvGlobalHistogramBuffer;
	m_csExclusiveSum.m_uavExclusivePrefixSumBuffer = m_buffer.m_uavExclusivePrefixSumBuffer;

	m_csDigitBinning.Initialize();
	m_csDigitBinning.m_settingsBuffer = m_buffer.m_settingsBuffer;
	m_csDigitBinning.m_exclusivePrefixSumBufferSrv = m_buffer.m_srvEclusivePrefixSumBuffer;

	m_csDigitBinning.m_uavMortonCodesBuffer = m_buffer.m_uavMortonCodeBuffer;
	m_csDigitBinning.m_uavPositionBuffer = m_buffer.m_uavPositionBuffer;
	m_csDigitBinning.m_uavMassBuffer = m_buffer.m_uavMassBuffer;

	m_csDigitBinning.m_srvMortonCodesBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csDigitBinning.m_srvPositionBuffer = m_buffer.m_srvPositionBuffer;
	m_csDigitBinning.m_srvMassBuffer = m_buffer.m_srvMassBuffer;

	m_csCornerStone_F.Initialize();
	m_csCornerStone_F.m_settingsBuffer = m_buffer.m_settingsBuffer;
	m_csCornerStone_F.m_mortonKeyBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csCornerStone_F.m_srvKBuffer = m_buffer.m_srvKBuffer;
	m_csCornerStone_F.m_uavNBuffer = m_buffer.m_uavNBuffer;
	//m_csDigitBinning.m_uavMassBuffer = m_uavMassBuffer;
	//m_csDigitBinning.m_uavPositionBuffer = m_uavPositionBuffer;
}


void ForceGeneratorBarnesHutCS::SetShaderParameter()
{
	if (m_deviceContext == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedPosition;
	D3D11_MAPPED_SUBRESOURCE mappedMass;

	HRESULT result = m_deviceContext->Map(m_buffer.m_upPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPosition);

	if (FAILED(result))
		return;

	result = m_deviceContext->Map(m_buffer.m_upMassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMass);

	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	XMFLOAT3* positionPtr = (XMFLOAT3*)mappedPosition.pData;
	float* massPtr = (float*)mappedMass.pData;

	for (int i = 0; i < m_objects.size() && i < MAX_CAPACITY; ++i)
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
	m_deviceContext->Unmap(m_buffer.m_upPositionBuffer, 0);
	m_deviceContext->Unmap(m_buffer.m_upMassBuffer, 0);

	m_deviceContext->CopyResource(m_buffer.m_inPositionBuffer, m_buffer.m_upPositionBuffer);
	m_deviceContext->CopyResource(m_buffer.m_inMassBuffer, m_buffer.m_upMassBuffer);

	m_csMortonCodes.SetShaderParameter(m_boundsMin, m_boundsMax);

	//Clear UAV
	m_buffer.ClearUavBuffer();
}

void ForceGeneratorBarnesHutCS::Exit()
{
	m_buffer.ReleaseShaderParameters();
	m_csMortonCodes.ShutdownShader();
	m_csUpfrontHistrogram.ShutdownShader();
	m_csExclusiveSum.ShutdownShader();
	m_csDigitBinning.ShutdownShader();
	m_csCornerStone_F.ShutdownShader();
}

void ForceGeneratorBarnesHutCS::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void ForceGeneratorBarnesHutCS::Dispatch()
{
	SetShaderParameter();
	//SetTestShaderParameter();
	uint32_t capacity = m_objects.size();

	CallculateMortonKeys(capacity);
	DispatchRadixSort (capacity);
	BuildOctree (capacity);

	m_deviceContext->Flush();
	m_deviceContext->CopyResource(m_buffer.m_outputBuffer, m_buffer.m_outMortonCodeBuffer);
	CallculateForces (capacity);
}

void ForceGeneratorBarnesHutCS::CallculateMortonKeys(uint32_t elementCount)
{
	//ToDo: reduce dispatch count!!!
	m_buffer.SetSettingParameter(elementCount, elementCount);
	m_csMortonCodes.Dispatch(elementCount, 1, 1);

	m_deviceContext->CopyResource(m_buffer.m_inMortonCodeBuffer, m_buffer.m_outMortonCodeBuffer);
}

void ForceGeneratorBarnesHutCS::DispatchRadixSort(uint32_t elementCount)
{
	//SM count for 3070 laptop is 40
	//We dispatch the kernel per SM core
	m_buffer.SetSettingParameter(elementCount, 40);
	m_csUpfrontHistrogram.Dispatch(40, 1, 1);

	m_buffer.SetSettingParameter(elementCount, 4);
	m_csExclusiveSum.Dispatch(4, 1, 1);

	uint32_t groupCount = (elementCount + 255) / 256;

	for (uint32_t i = 0; i < 4; ++i)
	{
		m_buffer.SetSettingParameter(elementCount, groupCount, i);
		m_csDigitBinning.Dispatch(groupCount, 1, 1);
		m_buffer.SwapParticleBuffer();
	}
}

void ForceGeneratorBarnesHutCS::BuildOctree(uint32_t elementCount)
{
	m_buffer.SetSettingParameter (elementCount, 2);
	m_csCornerStone_F.Dispatch(1,1,1);
}

void ForceGeneratorBarnesHutCS::CallculateForces(uint32_t elementCount)
{
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_csDigitBinning.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	
	CopyTestData();
}

void ForceGeneratorBarnesHutCS::CopyTestData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_buffer.m_outputBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		uint32_t data [1024];

		memcpy (&data[0], mappedResource.pData, sizeof(uint32_t) * 1024);

		m_deviceContext->Unmap(m_buffer.m_outputBuffer, 0);
	}
}

