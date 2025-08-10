#include "pch.h"
#include "DXContext.h"
#include "Windows/DXWindow.h"

bool DXContext::Init()
{
	__VERIFY_EXPR(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory)));

	IDXGIAdapter1* adapter;
	int adapterIndex = 0;
	bool adapterFound = false;
	while (m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		{
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
		{
			adapterFound = true;
			break;
		}
		adapterIndex++;
	}
	__VERIFY_EXPR(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	__VERIFY_EXPR(m_d3dDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)));

	if (!CreateSwapChain())
	{
		return false;
	}

	if (!CreateDepthSource())
	{
		return false;
	}

	CreateRTVHeap();
	CreateDSVHeap();

	if (!GetBuffers()) {
		return false;
	}

	__VERIFY_EXPR(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));
	__VERIFY_EXPR(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator, nullptr,IID_PPV_ARGS(&m_cmdList)));

	// 记录 GPU 渲染后返回的事件
	__VERIFY_EXPR(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_d3dFence)));
	m_fenceEvent = CreateEvent(NULL, false, false, NULL);

	return true;
}

void DXContext::Shutdown()
{
	ReleaseBuffers();
	
	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}
	m_d3d_ds_resoucre.Release();
	m_rtvDescHeap.Release();
	m_d3dFence.Release();
	m_cmdAllocator.Release();
	m_cmdList.Release();
	m_cmdQueue.Release();
	m_dxgiFactory.Release();
	m_d3dDevice.Release();
}

void DXContext::ReleaseBuffers()
{
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_buffers[i].Release();
	}
	//m_d3d_ds_resoucre.Release();
}

void DXContext::InitCommandList()
{
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, NULL);
}

void DXContext::ExeuteCommandList()
{
	if (SUCCEEDED(m_cmdList->Close()))
	{
		ID3D12CommandList* lists[] = { m_cmdList };
		m_cmdQueue->ExecuteCommandLists(1, lists);
		
		SignalAndWait();
	}
}

bool DXContext::CreateSwapChain()
{
	// Create Swap chain
	DXGI_SWAP_CHAIN_DESC scd{};
	scd.BufferCount = FrameCount;
	scd.BufferDesc = {};
	scd.BufferDesc.Width = DXWindow::Get().GetWidth();
	scd.BufferDesc.Height = DXWindow::Get().GetHeight();
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = DXWindow::Get().GetHWND();
	scd.SampleDesc.Count = 1;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	IDXGISwapChain* sc1 = nullptr;
	m_dxgiFactory->CreateSwapChain(m_cmdQueue, &scd, &sc1);
	m_swapChain = static_cast<IDXGISwapChain3*>(sc1);

	return true;
}

bool DXContext::CreateDepthSource()
{
	// 设置交换链 深度缓冲区
	D3D12_HEAP_PROPERTIES d3d_heap_properties = {};
	d3d_heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	
	D3D12_RESOURCE_DESC d3d12_Resource_desc{};
	d3d12_Resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d12_Resource_desc.Alignment = 0;
	d3d12_Resource_desc.Width = DXWindow::Get().GetWidth();
	d3d12_Resource_desc.Height = DXWindow::Get().GetHeight();
	d3d12_Resource_desc.DepthOrArraySize = 1;
	d3d12_Resource_desc.MipLevels = 1;
	d3d12_Resource_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d12_Resource_desc.SampleDesc.Count = 1;
	d3d12_Resource_desc.SampleDesc.Quality = 0;
	d3d12_Resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d12_Resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	
	__VERIFY_EXPR(m_d3dDevice->CreateCommittedResource(
		&d3d_heap_properties, 
		D3D12_HEAP_FLAG_NONE, 
		&d3d12_Resource_desc, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE, 
		&clearValue, 
		IID_PPV_ARGS(&m_d3d_ds_resoucre)
	));

	return true;
}

// 创建 RTV heap
void DXContext::CreateRTVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	//rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap));

	m_RTVDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void DXContext::UpdateRTVHeap() {
}

void DXContext::CreateDSVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvDescHeap));
	m_DSVDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

bool DXContext::GetBuffers()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapStart = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < FrameCount; i++) {
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvPointer;
		rtvPointer.ptr = rtvHeapStart.ptr + i * m_RTVDescriptorSize;
		m_d3dDevice->CreateRenderTargetView(m_buffers[i], nullptr, rtvPointer);
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC d3d_ds_view_desc{};
	d3d_ds_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d_ds_view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_d3dDevice->CreateDepthStencilView(m_d3d_ds_resoucre, &d3d_ds_view_desc, m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart());
	return true;
}

void DXContext::SignalAndWait()
{
	m_cmdQueue->Signal(m_d3dFence, ++m_fenceValue);

	if (m_d3dFence->GetCompletedValue() < m_fenceValue) {
		m_d3dFence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void DXContext::DrawFrame()
{
	m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

 	D3D12_RESOURCE_BARRIER barr{};
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr.Transition.pResource = m_buffers[m_currentBufferIndex];
	barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_cmdList->ResourceBarrier(1, &barr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv, dsv;
	dsv.ptr = m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart().ptr;
	rtv.ptr = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_currentBufferIndex * m_RTVDescriptorSize;
	m_cmdList->OMSetRenderTargets(1, &rtv, false, &dsv);

	D3D12_VIEWPORT viewport = { 0,0, DXWindow::Get().GetWidth(), DXWindow::Get().GetHeight() };
	D3D12_RECT scissorRect = { 0,0, DXWindow::Get().GetWidth(), DXWindow::Get().GetHeight()};
	m_cmdList->RSSetViewports(1, &viewport);
	m_cmdList->RSSetScissorRects(1, &scissorRect);

	float pColor[] = { m_backGroundColor.R, m_backGroundColor.G,m_backGroundColor.B, m_backGroundColor.A };

	m_cmdList->ClearRenderTargetView(rtv, pColor, 0, nullptr);
	m_cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void DXContext::EndFrame()
{
	D3D12_RESOURCE_BARRIER barr{};
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr.Transition.pResource = m_buffers[m_currentBufferIndex];
	barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	m_cmdList->ResourceBarrier(1, &barr);
}

void DXContext::Preset()
{
	m_swapChain->Present(0, 0);
}
