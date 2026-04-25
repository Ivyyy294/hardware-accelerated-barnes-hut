#include "pch.h"
#include "ShaderUtils.h"

bool ShaderUtils::CreateStagingBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer, UINT byteStride, UINT elementCount)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = 0;
	bufferDesc.ByteWidth = byteStride * elementCount;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = byteStride;

	result = device->CreateBuffer(&bufferDesc, NULL, buffer);

	return SUCCEEDED(result);
}

bool ShaderUtils::CreateDynamicBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer, UINT byteStride, UINT elementCount)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = byteStride * elementCount;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = byteStride;

	result = device->CreateBuffer(&bufferDesc, NULL, buffer);

	return SUCCEEDED(result);
}

bool ShaderUtils::CreateResourceBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer, UINT byteStride, UINT elementCount)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = byteStride * elementCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = byteStride;

	result = device->CreateBuffer(&bufferDesc, NULL, buffer);

	return SUCCEEDED(result);
}

bool ShaderUtils::CreateUAVBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11UnorderedAccessView** buffer, UINT elementCount)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = elementCount;

	result = device->CreateUnorderedAccessView(resource, &uavDesc, buffer);

	return SUCCEEDED(result);
}

bool ShaderUtils::CreateResourceViewBuffer(ID3D11Device* device, HRESULT& result, ID3D11Resource* resource, ID3D11ShaderResourceView** buffer, UINT elementCount)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = elementCount;

	result = device->CreateShaderResourceView(resource, &srvDesc, buffer);

	return SUCCEEDED(result);
}

bool ShaderUtils::CreateConstBuffer(ID3D11Device* device, HRESULT& result, ID3D11Buffer** buffer, UINT byteWidth)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = device->CreateBuffer(&bufferDesc, NULL, buffer);
	
	return SUCCEEDED(result);
}
