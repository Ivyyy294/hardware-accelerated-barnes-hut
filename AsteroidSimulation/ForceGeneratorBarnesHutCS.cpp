#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"

ForceGeneratorBarnesHutCS::ForceGeneratorBarnesHutCS()
	: ForceGeneratorBase()
	, m_deviceContext (D3D::Instance().GetDeviceContext())
	, m_buffer (m_maxParticleCount)
{
	const uint32_t maxCapacity = m_buffer.MaxCapacity();
	m_cellWidth = new float[maxCapacity];
	m_cellCenter = new XMFLOAT3 [maxCapacity];
	m_cellCom = new XMFLOAT3 [maxCapacity];
	m_cellCo = new uint32_t [maxCapacity];
	m_cellLc = new uint32_t [maxCapacity];
}

ForceGeneratorBarnesHutCS::~ForceGeneratorBarnesHutCS()
{
	delete[] m_cellWidth;
	delete[] m_cellCenter;
	delete[] m_cellCom;
	delete[] m_cellCo;
	delete[] m_cellLc;
}

void ForceGeneratorBarnesHutCS::Start()
{
	m_buffer.InitShaderParameters();

	m_csMortonCodes.Initialize();
	m_csMortonCodes.m_sharedBuffer = &m_buffer;

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
	m_csRadixSort_4.m_uavKeyBuffer = m_buffer.m_uavNkBuffer;

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

	m_csCornerStone_LO.Initialize();
	m_csCornerStone_LO.m_sharedBuffer = &m_buffer;

	m_csCornerStone_CO.Initialize();
	m_csCornerStone_CO.m_sharedBuffer = &m_buffer;

	m_csBarnesHut_MP.Initialize();
	m_csBarnesHut_MP.m_sharedBuffer = &m_buffer;

	m_csBarnesHut_CF.Initialize();
	m_csBarnesHut_CF.m_sharedBuffer = &m_buffer;

	Session::Me().m_shaderTimeFile.WriteLine("CSMortonCodes;RadixSort_MKey;CSCornerStone_F;CSCornerStone_G1;CSCornerStone_G2;CSCornerStone_G3;CSCornerStone_NK;RadixSort_NK;CSCornerStone_LO;CSCornerStone_CO;CSBarnesHut_MP;CSBarnesHut_CF");
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

	m_maxCoord = 0.f;

	for (int i = 0; i < m_objects.size(); ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		positionPtr[i].x = pos.x;
		positionPtr[i].y = pos.y;
		positionPtr[i].z = pos.z;
		massPtr[i] = m_objects[i]->GetMass();

		//ToDo: make bounding box tighter! We can make it smaller if the center is
		//not at 00
		m_maxCoord = max(m_maxCoord, std::abs(pos.x));
		m_maxCoord = max(m_maxCoord, std::abs(pos.y));
		m_maxCoord = max(m_maxCoord, std::abs(pos.z));
	}

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_buffer.m_upPositionBuffer, 0);
	m_deviceContext->Unmap(m_buffer.m_upMassBuffer, 0);

	m_deviceContext->CopyResource(m_buffer.m_inPositionBuffer, m_buffer.m_upPositionBuffer);
	m_deviceContext->CopyResource(m_buffer.m_inMassBuffer, m_buffer.m_upMassBuffer);

	m_maxCoord = std::ceilf(m_maxCoord);
	m_csMortonCodes.SetShaderParameter({ m_maxCoord, m_maxCoord, m_maxCoord });

	//Clear UAV
	m_buffer.ClearUavBuffer();

	//ensures that we run the rebalancing step once
	m_counterBuffer[2] = 1;
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

void ForceGeneratorBarnesHutCS::OnDrawGizmo()
{
	uint32_t count = m_counterBuffer[0] + m_counterBuffer[1];

	for (uint32_t i = 0; i < count; ++i)
	{
		if (m_cellCo[i] == 0 && m_cellLc[i] > 0)
		{
			Vector3 center (m_cellCenter[i].x, m_cellCenter[i].y, m_cellCenter[i].z);

			Vector3 halfExtends (m_cellWidth[i] * 0.5f, m_cellWidth[i] * 0.5f, m_cellWidth[i] * 0.5f);
			Gizmo::AddCube (center, halfExtends, Color::Green, Transform::Space::WORLD);

			Vector3 com (m_cellCom[i].x, m_cellCom[i].y, m_cellCom[i].z);
			Gizmo::AddCube (com, Vector3(0.1f, 0.1f, 0.1f), Color::Red, Transform::Space::WORLD);

			Gizmo::AddLine (center, com, Color::Red, Transform::Space::WORLD);
		}
	}
}

void ForceGeneratorBarnesHutCS::PreCalculateForcesInternal()
{
	SetShaderParameter();
	//SetTestShaderParameter();
	uint32_t capacity = m_objects.size();

	CallculateMortonKeys(capacity);
	DispatchRadixSort (capacity);
	BuildOctree (capacity);

	CallculateForces (capacity);

	CopyForceData();

	if (Gizmo::Draw)
		CopyCellData();

	if (Session::Me().m_logDebugInfo)
	{
		RecordExecutionTime();
		m_buffer.RecordBufferSnapshot();
	}
}

void ForceGeneratorBarnesHutCS::CallculateMortonKeys(uint32_t elementCount)
{
	uint32_t groupCount = 40;
	m_buffer.SetSettingParameter(elementCount, groupCount);
	m_csMortonCodes.Dispatch(groupCount, 1, 1);

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

	uint32_t groupCount = (elementCount + 255) / 256;
	
	//Assure we run rebalancing loop once
	CopyCounterBuffer();
	m_counterBuffer[2] = 1;

	for (uint32_t i = 0; i < 10 && m_counterBuffer[2] > 0; ++i)
	{
		m_buffer.SetSettingParameter (elementCount, 0);
		m_csCornerStone_F.Dispatch(groupCount,1,1);

		m_buffer.SetSettingParameter(m_nCrit, 0);
		m_csCornerStone_G1.Dispatch(groupCount, 1, 1);

		m_csCornerStone_G2.Dispatch(groupCount, 1, 1);
		m_csCornerStone_G3.Dispatch(groupCount, 1, 1);

		m_deviceContext->CopyResource(m_buffer.m_inKBuffer, m_buffer.m_outKBuffer);
		CopyCounterBuffer();
	}

	m_csCornerStone_NK.Dispatch(groupCount, 1, 1);
	CopyCounterBuffer();

	m_deviceContext->CopyResource(m_buffer.m_inNkBuffer, m_buffer.m_outNkBuffer);

	//record sort time before sorting nk
	if (Session::Me().m_logDebugInfo)
	{
		m_sortTimeKeyMs = m_csRadixSort_1.GetExecutionTimeMs()
		+ m_csRadixSort_2.GetExecutionTimeMs()
		+ m_csRadixSort_3.GetExecutionTimeMs();
	}

	//Sort NK
	uint32_t nCount = m_counterBuffer[0] + m_counterBuffer[1];
	m_buffer.SetSettingParameter(nCount, 40);
	m_csRadixSort_1.m_srvKeyBuffer = m_buffer.m_srvNkBuffer;
	m_csRadixSort_1.Dispatch(40, 1, 1);

	m_buffer.SetSettingParameter(nCount, 4);
	m_csRadixSort_2.Dispatch(4, 1, 1);

	groupCount = (nCount + 1 + 255) / 256;

	for (uint32_t i = 0; i < 4; ++i)
	{
		m_buffer.SetSettingParameter(nCount, groupCount, i);
		m_csRadixSort_4.Dispatch(groupCount, 1, 1);
		m_deviceContext->CopyResource(m_buffer.m_inNkBuffer, m_buffer.m_outNkBuffer);
	}

	m_csCornerStone_LO.Dispatch(1,1,1);
	m_csCornerStone_CO.Dispatch(groupCount,1,1);
}

void ForceGeneratorBarnesHutCS::CallculateForces(uint32_t elementCount)
{
	uint32_t nCount = m_counterBuffer[0] + m_counterBuffer[1];
	uint32_t groupCount = (elementCount + 255) / 256;
	m_buffer.SetBhSettingsParameter (elementCount, m_maxCoord * 2.f, IvyyyPhysic::GravitationalConstant(), m_theta);
	m_csBarnesHut_MP.Dispatch(nCount, 1, 1);
	m_csBarnesHut_CF.Dispatch(nCount, 1, 1);
}

void ForceGeneratorBarnesHutCS::CopyTestData()
{
	m_deviceContext->CopyResource(m_buffer.m_outputBuffer, m_buffer.m_debugBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_buffer.m_outputBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		const uint32_t size = 1024;
		uint32_t data [size];

		memcpy (&data[0], mappedResource.pData, sizeof(uint32_t) * size);
		m_deviceContext->Unmap(m_buffer.m_outputBuffer, 0);
	}
}

void ForceGeneratorBarnesHutCS::CopyForceData()
{
	//Copy Force
	m_deviceContext->CopyResource(m_buffer.m_outForceBuffer, m_buffer.m_forceBuffer);

	//Determine nk size 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_buffer.m_outForceBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		XMFLOAT3* dataPtr = (XMFLOAT3*)mappedResource.pData;
		XMFLOAT3 test = dataPtr[m_maxParticleCount-1];
		memcpy(m_forceBuffer, dataPtr, sizeof(XMFLOAT3) * (m_maxParticleCount));
		m_deviceContext->Unmap(m_buffer.m_outForceBuffer, 0);
	}
}

void ForceGeneratorBarnesHutCS::CopyCounterBuffer()
{
	//Determine nk size 
	m_deviceContext->CopyResource(m_buffer.m_stagingCounterBuffer, m_buffer.m_counterBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_buffer.m_stagingCounterBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		memcpy(&m_counterBuffer[0], mappedResource.pData, sizeof(uint32_t) * 3);
		m_deviceContext->Unmap(m_buffer.m_stagingCounterBuffer, 0);
	}
}

void ForceGeneratorBarnesHutCS::CopyCellData()
{
	//Copy cellWidth
	m_deviceContext->CopyResource(m_buffer.m_dFloatBuffer, m_buffer.m_cellWidthBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(m_buffer.m_dFloatBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	const uint32_t maxCapacity = m_buffer.MaxCapacity();

	if (SUCCEEDED(hr))
	{
		float* dataPtr = (float*)mappedResource.pData;
		memcpy(m_cellWidth, dataPtr, sizeof(float) * maxCapacity);
		m_deviceContext->Unmap(m_buffer.m_dFloatBuffer, 0);
	}

	//Copy cell center
	m_deviceContext->CopyResource(m_buffer.m_dFloat3Buffer, m_buffer.m_cellCenterBuffer);

	mappedResource;
	hr = m_deviceContext->Map(m_buffer.m_dFloat3Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		XMFLOAT3* dataPtr = (XMFLOAT3*)mappedResource.pData;
		memcpy(m_cellCenter, dataPtr, sizeof(XMFLOAT3) * maxCapacity);
		m_deviceContext->Unmap(m_buffer.m_dFloat3Buffer, 0);
	}

	//Copy cell com
	m_deviceContext->CopyResource(m_buffer.m_dFloat3Buffer, m_buffer.m_centerMassBuffer);

	mappedResource;
	hr = m_deviceContext->Map(m_buffer.m_dFloat3Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		XMFLOAT3* dataPtr = (XMFLOAT3*)mappedResource.pData;
		memcpy(m_cellCom, dataPtr, sizeof(XMFLOAT3) * maxCapacity);
		m_deviceContext->Unmap(m_buffer.m_dFloat3Buffer, 0);
	}

	//Copy cell co
	m_deviceContext->CopyResource(m_buffer.m_dUint32Buffer, m_buffer.m_coBuffer);

	mappedResource;
	hr = m_deviceContext->Map(m_buffer.m_dUint32Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		uint32_t* dataPtr = (uint32_t*)mappedResource.pData;
		memcpy(m_cellCo, dataPtr, sizeof(uint32_t) * maxCapacity);
		m_deviceContext->Unmap(m_buffer.m_dUint32Buffer, 0);
	}

	//Copy cell lc
	m_deviceContext->CopyResource(m_buffer.m_dUint32Buffer, m_buffer.m_lcBuffer);

	mappedResource;
	hr = m_deviceContext->Map(m_buffer.m_dUint32Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		uint32_t* dataPtr = (uint32_t*)mappedResource.pData;
		memcpy(m_cellLc, dataPtr, sizeof(uint32_t) * maxCapacity);
		m_deviceContext->Unmap(m_buffer.m_dUint32Buffer, 0);
	}
}

void ForceGeneratorBarnesHutCS::RecordExecutionTime()
{
	float sortTimeKeyMs = m_csRadixSort_1.GetExecutionTimeMs()
		+ m_csRadixSort_2.GetExecutionTimeMs()
		+ m_csRadixSort_4.GetExecutionTimeMs();;

	string val = std::to_string(m_csMortonCodes.GetExecutionTimeMs())
		+ ";" + std::to_string(m_sortTimeKeyMs)
		+ ";" + std::to_string(m_csCornerStone_F.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csCornerStone_G1.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csCornerStone_G2.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csCornerStone_G3.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csCornerStone_NK.GetExecutionTimeMs())
		+ ";" + std::to_string(sortTimeKeyMs)
		+ ";" + std::to_string(m_csCornerStone_LO.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csCornerStone_CO.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csBarnesHut_MP.GetExecutionTimeMs())
		+ ";" + std::to_string(m_csBarnesHut_CF.GetExecutionTimeMs());

	Session::Me().m_shaderTimeFile.WriteLine(val);
}

