#pragma once

#include <IvyyyComputeShader.h>

class BarnesHutCS :
    public ComputeShader
{
//Variables
	const int m_maxSize {1};

	ID3D11Buffer* m_inputBuffer{ nullptr };
	ID3D11ShaderResourceView* m_inputBufferSrv = nullptr;

	ID3D11Buffer* m_outputBuffer{ nullptr };
	ID3D11Buffer* m_outputResultBuffer{ nullptr };
	ID3D11UnorderedAccessView* m_outputBufferUav = nullptr;

//Methods
public:
	BarnesHutCS();
	void ShutdownShader() override;

	void CopyData();

private:
	bool BindShaderParameters (ID3D11DeviceContext* deviceContext) override;
	void UnbindShaderParameters(ID3D11DeviceContext* deviceContext) override;
	bool InitShaderParameters (ID3D11Device* device) override;
};

