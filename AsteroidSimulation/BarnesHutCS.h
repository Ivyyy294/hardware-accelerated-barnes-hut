#pragma once

#include <IvyyyComputeShader.h>

class BarnesHutCS :
    public ComputeShader
{
private:
//Definitions
	struct UnitCube
	{
		XMFLOAT3 boundsMax;
		XMFLOAT3 boundsMin;
		double padding;
	};

//Variables
	int m_maxSize {1};
	Vector3 m_boundingVolume {Vector3::Zero};

	ID3D11Buffer* m_unitCubeBuffer {nullptr};

	ID3D11Buffer* m_positionBuffer{ nullptr };
	ID3D11ShaderResourceView* m_positionBufferSrv = nullptr;

	ID3D11Buffer* m_massBuffer{ nullptr };
	ID3D11ShaderResourceView* m_massBufferSrv = nullptr;

	ID3D11Buffer* m_outputBuffer{ nullptr };
	ID3D11Buffer* m_outputResultBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_outputBufferUav = nullptr;
	
//Methods
public:
	BarnesHutCS();
	void ShutdownShader() override;

	void CopyData();
	inline void SetMaxSize (int size) {m_maxSize = size;};

	void SetShaderParameters(const std::vector<PhysicObject*>& objects);

private:
	bool BindShaderParameters (ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters (ID3D11Device* device) override;
};

