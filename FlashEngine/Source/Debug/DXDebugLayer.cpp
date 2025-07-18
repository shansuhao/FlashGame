#include "pch.h"
#include "DXDebugLayer.h"

bool DXDebugLayer::Init()
{
    __VERIFY_EXPR(D3D12GetDebugInterface(IID_PPV_ARGS(m_d3d12Debug.GetAddressOf())));
    m_d3d12Debug->EnableDebugLayer();
    __VERIFY_EXPR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_dxgiDebug.GetAddressOf())));
    m_dxgiDebug->EnableLeakTrackingForThread();

    return true;
}
