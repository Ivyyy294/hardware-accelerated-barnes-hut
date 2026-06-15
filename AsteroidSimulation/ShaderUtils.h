#pragma once

class ShaderUtils
{
//Methods
public:
	static bool CreateStagingBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount);

	static bool CreateDynamicBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount, unsigned int miscFlag = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

	static bool CreateResourceBuffer (ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount, unsigned int miscFlag = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, D3D11_SUBRESOURCE_DATA* initalData = NULL);

	static bool CreateUAVBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11UnorderedAccessView** buffer
		, UINT elementCount, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	static bool CreateResourceViewBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11ShaderResourceView** buffer
		, UINT elementCount, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	static bool CreateConstBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer, UINT byteWidth);

	template<class Interface>
	static inline void SafeRelease(Interface** ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != NULL)
		{
			(*ppInterfaceToRelease)->Release();
			(*ppInterfaceToRelease) = NULL;
		}
	}
};

