#pragma once

#include "CSSettings.h"

class CsSharedBuffer
{
//Variables
private:
	ID3D11DeviceContext* m_deviceContext{ nullptr };

public:
	//Debug buffer
	ID3D11Buffer* m_outputBuffer{ nullptr };
	ID3D11Buffer* m_debugBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavDebugBuffer = nullptr;

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
	ID3D11UnorderedAccessView* m_uavCounterBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvCounterBuffer = nullptr;

	ID3D11Buffer* m_inNkBuffer{ nullptr };
	ID3D11Buffer* m_outNkBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_uavNkBuffer = nullptr;
	ID3D11ShaderResourceView* m_srvNkBuffer = nullptr;

//Methods
public:
	CsSharedBuffer();

	bool InitShaderParameters();
	void ReleaseShaderParameters();
	void SetSettingParameter(uint32_t elementCount, uint32_t dispatchGroupCount, uint32_t digitPlace = 0);

	void ClearUavBuffer();
	void SwapParticleBuffer ();

private:
	HRESULT InitMortonCodeShaderParameters(ID3D11Device* device);
	HRESULT InitPositionShaderParameters(ID3D11Device* device);
	HRESULT InitMassShaderParameters(ID3D11Device* device);
	HRESULT InitSortingShaderParameters(ID3D11Device* device);
	HRESULT InitCornerStoneShaderParameters(ID3D11Device* device);

};

