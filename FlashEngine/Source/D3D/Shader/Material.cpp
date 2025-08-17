#include "pch.h"
#include "Material.h"
#include "D3D/DXContext.h"

Material::Material()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 32;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXContext::Get().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void Material::SetTexture2D(int inSRVIndex, ComPointer<ID3D12Resource>& inResource, int inMipMapLevelCount, DXGI_FORMAT inFormat)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = inFormat;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = inMipMapLevelCount;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapPtr = m_srvHeap->GetCPUDescriptorHandleForHeapStart();

	srvHeapPtr.ptr += inSRVIndex * DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DXContext::Get().GetDevice()->CreateShaderResourceView(inResource.Get(), &srvDesc, srvHeapPtr);
}

void Material::SetStructuredBuffer(int inSRVIndex, ComPointer<ID3D12Resource>& inResource, int inPerElementSize, int inElementCount)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC sbSRVDesc = {};
	sbSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sbSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	sbSRVDesc.Buffer.FirstElement = 0;
	sbSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	sbSRVDesc.Buffer.NumElements = inElementCount;
	sbSRVDesc.Buffer.StructureByteStride = inPerElementSize;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapPtr = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvHeapPtr.ptr += inSRVIndex * DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DXContext::Get().GetDevice()->CreateShaderResourceView(inResource.Get(), &sbSRVDesc, srvHeapPtr);
}
