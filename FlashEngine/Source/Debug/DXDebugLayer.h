#pragma once
#include "D3D/D3DLibary.h"

class DXDebugLayer
{
public:
	bool Init();

private:
#ifdef _DEBUG
	ComPtr<ID3D12Debug6> m_d3d12Debug;
	ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif // _DEBUG

public:
	DXDebugLayer(const DXDebugLayer&) = delete;
	DXDebugLayer operator=(const DXDebugLayer&) = delete;

	inline static DXDebugLayer& Get() {
		static DXDebugLayer instance;
		return instance;
	}
private:
	DXDebugLayer() = default;
};

