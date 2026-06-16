#include "pch.h"
#include "UavSrvBuffer.h"
#include "ShaderUtils.h"

bool UavSrvBuffer::Init(ID3D11Device* device, HRESULT& result, UINT byteStride, UINT elementCount, UINT miscFlag, DXGI_FORMAT format)
{
	bool ok = ShaderUtils::CreateResourceBuffer(device, result, &m_buffer, byteStride, elementCount, miscFlag)
		&& ShaderUtils::CreateResourceViewBuffer(device, result, m_buffer, &m_srv, elementCount, format)
		&& ShaderUtils::CreateUAVBuffer(device, result, m_buffer, &m_uav, elementCount, format);

	return ok;
}

void UavSrvBuffer::Release()
{
	ShaderUtils::SafeRelease(&m_buffer);
	ShaderUtils::SafeRelease(&m_srv);
	ShaderUtils::SafeRelease(&m_uav);
}