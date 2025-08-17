#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

struct MaterialData {
	float mDiffuseColor[4];
	float mSpecularColor[4];
};

class Material
{
public:
	ComPointer<ID3D12Resource> m_ConstantBuffer;
	ComPointer<ID3D12Resource> m_StructuredBuffer;
	ComPointer<ID3D12DescriptorHeap> m_srvHeap;
	ComPointer<ID3D12PipelineState> m_PipeState;

	Material();

	void SetTexture2D(int inSRVIndex, ComPointer<ID3D12Resource>& inResource, int inMipMapLevelCount = 1, DXGI_FORMAT inFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	void SetStructuredBuffer(int inSRVIndex, ComPointer<ID3D12Resource>& inResource, int inPerElementSize = 0, int inElementCount = 0);
	void Active();

	void Test();
};

