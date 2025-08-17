#include "pch.h"
#include "Material.h"

#include "Utils/Utils.h"
#include "D3D/DXContext.h"

#define StructuredBufferIndexStart 16

Material::Material()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 32;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXContext::Get().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

	DXContext::Get().CreateConstantBufferOBject(m_ConstantBuffer, 65536);
	DXContext::Get().CreateConstantBufferOBject(m_StructuredBuffer, 65536);
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

void Material::Active()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap };

	DXContext::Get().GetCommandList()->SetPipelineState(m_PipeState);
	DXContext::Get().GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DXContext::Get().GetCommandList()->SetGraphicsRootConstantBufferView(1, m_ConstantBuffer->GetGPUVirtualAddress());
	DXContext::Get().GetCommandList()->SetGraphicsRootDescriptorTable(2, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
	DXContext::Get().GetCommandList()->SetGraphicsRootShaderResourceView(3, m_StructuredBuffer->GetGPUVirtualAddress());
}

void Material::Test()
{
	float* materialDatas = new float[3000];
	for (int i = 0; i < 3000; i++)
	{
		materialDatas[i] = Flash::srandom() * 0.1f + 0.1f;
	}
	DXContext::Get().UpdateConstantBuffer(m_StructuredBuffer, materialDatas, sizeof(float) * 3000);
	SetStructuredBuffer(StructuredBufferIndexStart, m_StructuredBuffer, sizeof(float), 3000);
}
