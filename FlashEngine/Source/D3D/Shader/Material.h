#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

class Material
{
public:
	ComPointer<ID3D12Resource> m_ConstantBuffer;
	ComPointer<ID3D12Resource> m_StructuredBuffer;
	ComPointer<ID3D12DescriptorHeap> m_srvHeap;
	ComPointer<ID3D12PipelineState> m_PipeState;

	Material();
};

