#pragma once

#define MAX_CAPACITY 1024

class ShaderUtils
{
//Methods
public:
	static bool CreateStagingBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount);


	static bool CreateDynamicBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount);


	static bool CreateResourceBuffer (ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer
		, UINT byteStride, UINT elementCount);


	static bool CreateUAVBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11UnorderedAccessView** buffer
		, UINT elementCount);


	static bool CreateResourceViewBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11ShaderResourceView** buffer
		, UINT elementCount);

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

