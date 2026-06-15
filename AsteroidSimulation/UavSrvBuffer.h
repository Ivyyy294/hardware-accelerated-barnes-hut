#pragma once

class UavSrvBuffer
{
//Variables
public:
	ID3D11Buffer* m_buffer{ nullptr };
	ID3D11UnorderedAccessView* m_uav = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
//Methods
public:
	bool Init(ID3D11Device* device, HRESULT& result, UINT byteStride, UINT elementCount, uint32_t miscFlag = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
	void Release();
};

