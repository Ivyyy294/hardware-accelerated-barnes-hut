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

	m_csRadixSort_1.Initialize();
	m_csRadixSort_1.m_sharedBuffer = &m_buffer;

	m_csRadixSort_2.Initialize();
	m_csRadixSort_2.m_sharedBuffer = &m_buffer;

	m_csRadixSort_3.Initialize();
	m_csRadixSort_3.m_sharedBuffer = &m_buffer;
	m_csRadixSort_3.m_uavKeyBuffer = m_buffer.m_uavMortonCodeBuffer;
	m_csRadixSort_3.m_uavVal1Buffer = m_buffer.m_uavPositionBuffer;
	m_csRadixSort_3.m_uavVal2Buffer = m_buffer.m_uavMassBuffer;
	m_csRadixSort_3.m_srvKeyBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csRadixSort_3.m_srvVal1Buffer = m_buffer.m_srvPositionBuffer;
	m_csRadixSort_3.m_srvVal2Buffer = m_buffer.m_srvMassBuffer;

	m_csRadixSort_4.Initialize();
	m_csRadixSort_4.m_sharedBuffer = &m_buffer;
	m_csRadixSort_4.m_srvKeyBuffer = m_buffer.m_srvNkBuffer;
	//m_csRadixSort_4.m_uavKeyBuffer = m_buffer.m_uavNkBuffer;

	m_csCornerStone_F.Initialize();
	m_csCornerStone_F.m_sharedBuffer = &m_buffer;

	m_csCornerStone_G1.Initialize();
	m_csCornerStone_G1.m_sharedBuffer = &m_buffer;

	m_csCornerStone_G2.Initialize();
	m_csCornerStone_G2.m_sharedBuffer = &m_buffer;

	m_csCornerStone_G3.Initialize();
	m_csCornerStone_G3.m_sharedBuffer = &m_buffer;

	m_csCornerStone_NK.Initialize();
	m_csCornerStone_NK.m_sharedBuffer = &m_buffer;
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
	m_csRadixSort_1.ShutdownShader();
	m_csRadixSort_2.ShutdownShader();
	m_csRadixSort_3.ShutdownShader();
	m_csRadixSort_4.ShutdownShader();
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
	CallculateForces (capacity);

	m_deviceContext->CopyResource(m_buffer.m_outputBuffer, m_buffer.m_debugBuffer);
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
	m_csRadixSort_1.m_srvKeyBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csRadixSort_1.Dispatch(40, 1, 1);

	m_buffer.SetSettingParameter(elementCount, 4);
	m_csRadixSort_2.Dispatch(4, 1, 1);

	uint32_t groupCount = (elementCount + 255) / 256;

	for (uint32_t i = 0; i < 4; ++i)
	{
		m_buffer.SetSettingParameter(elementCount, groupCount, i);
		m_csRadixSort_3.Dispatch(groupCount, 1, 1);
		m_buffer.SwapParticleBuffer();
	}
}

void ForceGeneratorBarnesHutCS::BuildOctree(uint32_t elementCount)
{
	//Repeat max L times, L = 10

	uint32_t groupCount = (MAX_CAPACITY + 1 + 255) / 256;

	for (uint32_t i = 0; i < 10; ++i)
	{
		m_buffer.SetSettingParameter (elementCount, 0);
		m_csCornerStone_F.Dispatch(groupCount,1,1);

		m_buffer.SetSettingParameter(m_nCrit, 0);
		m_csCornerStone_G1.Dispatch(groupCount, 1, 1);

		m_csCornerStone_G2.Dispatch(groupCount, 1, 1);

		m_csCornerStone_G3.Dispatch(groupCount, 1, 1);


		m_deviceContext->CopyResource(m_buffer.m_inKBuffer, m_buffer.m_outKBuffer);
	}
	
	m_csCornerStone_NK.Dispatch(groupCount, 1, 1);

	//m_deviceContext->CopyResource(m_inMortonCodeBuffer, m_buffer.nk);

	//Sort NK
	elementCount = 33;
	m_buffer.SetSettingParameter(elementCount, 40);
	m_csRadixSort_1.m_srvKeyBuffer = m_buffer.m_srvMortonCodeBuffer;
	m_csRadixSort_1.Dispatch(40, 1, 1);

	m_buffer.SetSettingParameter(elementCount, 4);
	m_csRadixSort_2.Dispatch(4, 1, 1);

	groupCount = (elementCount + 255) / 256;

	for (uint32_t i = 0; i < 4; ++i)
	{
		m_buffer.SetSettingParameter(elementCount, groupCount, i);
		m_csRadixSort_4.Dispatch(groupCount, 1, 1);
	}
}

void ForceGeneratorBarnesHutCS::CallculateForces(uint32_t elementCount)
{
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_csCornerStone_NK.IsDone())
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
		const uint32_t size = MAX_CAPACITY + MAX_CAPACITY;
		uint32_t data [size];

		memcpy (&data[0], mappedResource.pData, sizeof(uint32_t) * size);

		m_deviceContext->Unmap(m_buffer.m_outputBuffer, 0);
	}
}

