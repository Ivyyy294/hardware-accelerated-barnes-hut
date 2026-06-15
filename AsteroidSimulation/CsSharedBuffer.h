#pragma once

#include "CSSettings.h"
#include "Session.h"

struct BhSettings
{
	uint32_t pCount;
	float globalCellWidth;
	float ugc;
	float theta;
};

class CsSharedBuffer
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext{ nullptr };
	uint32_t m_maxParticleCount = 0;
	uint32_t m_maxCapacity = 0;

public:
	//Debug buffer
	ID3D11Buffer* m_outputBuffer{ nullptr };
	ID3D11Buffer* m_debugBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavDebugBuffer = nullptr;

	ID3D11Buffer* m_dUint32Buffer{ nullptr };
	ID3D11Buffer* m_dFloatBuffer{ nullptr };
	ID3D11Buffer* m_dFloat4Buffer{ nullptr };

	//Settings buffer
	ID3D11Buffer* m_settingsBuffer{ nullptr };

	//Morton Code Buffer
	ID3D11Buffer* m_upMortonCodeBuffer{ nullptr };
	ID3D11Buffer* m_inMortonCodeBuffer{ nullptr };
	ID3D11Buffer* m_outMortonCodeBuffer{ nullptr };
	ID3D11ShaderResourceView* m_srvMortonCodeBuffer = nullptr;
	ID3D11UnorderedAccessView* m_uavMortonCodeBuffer = nullptr;

	//Position Buffer
	ID3D11Buffer* m_upPositionBuffer{ nullptr };
	ID3D11Buffer* m_inPositionBuffer{ nullptr };
	ID3D11Buffer* m_outPositionBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavPositionBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvPositionBuffer = nullptr;

	//Mass Buffer
	ID3D11Buffer* m_upMassBuffer{ nullptr };
	ID3D11Buffer* m_inMassBuffer{ nullptr };
	ID3D11Buffer* m_outMassBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavMassBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvMassBuffer = nullptr;

	//New To Old index buffer
	ID3D11Buffer* m_inNewToOldIndexBuffer{ nullptr };
	ID3D11Buffer* m_outNewToOldIndexBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavNewToOldIndexBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvNewToOldIndexBuffer = nullptr;

	//Sorting Buffer
	ID3D11Buffer* m_globalHistogramBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavGlobalHistogramBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvGlobalHistogramBuffer = nullptr;

	ID3D11Buffer* m_exclusivePrefixSumBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavExclusivePrefixSumBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvEclusivePrefixSumBuffer = nullptr;

	//Corner stone buffer
	ID3D11Buffer* m_inKBuffer{ nullptr };
	ID3D11ShaderResourceView* m_srvKBuffer = nullptr;
	ID3D11Buffer* m_outKBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavKBuffer = nullptr;

	//Leaf node particle count
	ID3D11Buffer* m_nBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavNBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvNBuffer = nullptr;

	//Auxiliary array for node transformations
	ID3D11Buffer* m_o1Buffer{ nullptr };
	ID3D11ShaderResourceView* m_srvO1Buffer = nullptr;
	ID3D11UnorderedAccessView* m_uavO1Buffer = nullptr;

	ID3D11Buffer* m_o2Buffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavO2Buffer = nullptr;
	ID3D11ShaderResourceView* m_srvO2Buffer = nullptr;

	ID3D11Buffer* m_counterBuffer{ nullptr };
	ID3D11Buffer* m_stagingCounterBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavCounterBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCounterBuffer = nullptr;

	ID3D11Buffer* m_inNkBuffer{ nullptr };
	ID3D11Buffer* m_outNkBuffer{ nullptr };
	ID3D11Buffer* m_stagingNkBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavNkBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvNkBuffer = nullptr;

	ID3D11Buffer* m_loBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavLoBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvLoBuffer = nullptr;

	ID3D11Buffer* m_coBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavCoBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCoBuffer = nullptr;

	ID3D11Buffer* m_liBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavLiBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvLiBuffer = nullptr;

	ID3D11Buffer* m_lcBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavLcBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvLcBuffer = nullptr;

	//Barnes hut buffer
	ID3D11Buffer* m_bhSettingsBuffer{ nullptr };

	ID3D11Buffer* m_cellMpDataBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavCellMpDataBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCellMpDataBuffer = nullptr;

	ID3D11Buffer* m_forceBuffer{ nullptr };
	ID3D11Buffer* m_outForceBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavForceBuffer = nullptr;

	ID3D11Buffer* m_cellGeoDataBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavCellGeoDataBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCellGeoDataBuffer = nullptr;

//Methods
public:
	CsSharedBuffer(uint32_t maxParticleCount);

	bool InitShaderParameters();
	void ReleaseShaderParameters();
	void SetSettingParameter(uint32_t elementCount, uint32_t dispatchGroupCount, uint32_t digitPlace = 0);
	void SetBhSettingsParameter(uint32_t elementCount, float cellWidth, float ugc, float theta);
	void ClearUavBuffer();
	void SwapParticleBuffer ();

	void RecordBufferSnapshot();

	inline uint32_t MaxCapacity() const {return m_maxCapacity;}

private:
	HRESULT InitDebugShaderParameters(ID3D11Device* device);
	HRESULT InitMortonCodeShaderParameters(ID3D11Device* device);
	HRESULT InitPositionShaderParameters(ID3D11Device* device);
	HRESULT InitIndexMappingShaderParameters (ID3D11Device* device);
	HRESULT InitMassShaderParameters(ID3D11Device* device);
	HRESULT InitSortingShaderParameters(ID3D11Device* device);
	HRESULT InitCornerStoneShaderParameters(ID3D11Device* device);
	HRESULT InitBarnesHutShaderParameters(ID3D11Device* device);

	template <typename T>
	void RecordBufferSnapshot (ID3D11Buffer* dest, ID3D11Buffer* source, const string name, const size_t count);

	void RecordBufferSnapshotXMFLOAT4 (ID3D11Buffer* dest, ID3D11Buffer* source, const string name, const size_t count, int valIndex);
};

template<typename T>
void CsSharedBuffer::RecordBufferSnapshot(ID3D11Buffer* dest, ID3D11Buffer* source, const string name, const size_t count)
{
	//m_deviceContext->CopyResource (dest, source);
	m_deviceContext->CopySubresourceRegion (dest, 0, 0,0,0, source, 0, nullptr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_deviceContext->Map(dest, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		T* data = (T*)mappedResource.pData;

		string memDump = name + ";";
		memDump.reserve (sizeof(T) * count);

		for (size_t i = 0; i < count; ++i)
			memDump += std::to_string(data[i]) + ";";

		Session::Me().m_gpuMemDumpFile.WriteLine (memDump);

		m_deviceContext->Unmap(dest, 0);
	}
}
