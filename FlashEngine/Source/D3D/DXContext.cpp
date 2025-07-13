#include "pch.h"
#include "DXContext.h"
#include "Windows/DXWindow.h"

/*
	初始化流程
	
	创建DXGI
	创建D3d12设备对象
	创建命令队列
	创建围栏
	创建交换链
	创建描述符堆
	获取交换链数据
*/
bool DXContext::Init()
{
	__VERIFY_EXPR(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory)));
	__VERIFY_EXPR(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{}; 
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	__VERIFY_EXPR(m_d3dDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)));
	
	__VERIFY_EXPR(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_d3dFence)));
	m_fenceEvent = CreateEvent(NULL, false, false, NULL);

	__VERIFY_EXPR(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));
	__VERIFY_EXPR(m_d3dDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList)));

	CreateRTVHeap();
	if (!CreateSwapChain())
	{
		return false;
	}
	
	if (!GetBuffers()) {
		return false;
	}

	return true;
}

void DXContext::Shutdown()
{
	ReleaseBuffers();

	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}

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
}

ID3D12GraphicsCommandList7* DXContext::InitCommandList()
{
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator.Get(), NULL);
	return m_cmdList;
}

void DXContext::ExeuteCommandList()
{
	if (SUCCEEDED(m_cmdList->Close()))
	{
		ID3D12CommandList* lists[] = {m_cmdList};
		m_cmdQueue->ExecuteCommandLists(1, lists);
	}
}

bool DXContext::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swd{};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};

	swd.Width = DXWindow::Get().GetWidth();
	swd.Height = DXWindow::Get().GetWidth();
	swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swd.Stereo = false;
	swd.SampleDesc.Count = 1;
	swd.SampleDesc.Quality = 0;
	swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swd.BufferCount = FrameCount;
	swd.Scaling = DXGI_SCALING_STRETCH;
	swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	sfd.Windowed = true;

	// Create Swap chain
	ComPointer<IDXGISwapChain1> sc1;
	m_dxgiFactory->CreateSwapChainForHwnd(m_cmdQueue, DXWindow::Get().GetHWND(), &swd, &sfd, nullptr, &sc1);

	__VERIFY_EXPR(!sc1->QueryInterface(&m_swapChain));

	return true;
}

// 创建 RTV heap
void DXContext::CreateRTVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap));

	auto fristHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto handleIncrement = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_rtvHandles[i] = fristHandle;
		m_rtvHandles[i].ptr += handleIncrement * i;
	}
}

bool DXContext::GetBuffers()
{
	for (size_t i = 0; i < FrameCount; i++)
	{
		__VERIFY_EXPR(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])));

		D3D12_RENDER_TARGET_VIEW_DESC rtv{};
		rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtv.Texture2D.MipSlice = 0;
		rtv.Texture2D.PlaneSlice = 0;
		m_d3dDevice->CreateRenderTargetView(m_buffers[i], &rtv, m_rtvHandles[i]);
	}
	return true;
}

void DXContext::DrawFrame(ID3D12GraphicsCommandList7* cmdList)
{
	m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barr;
	barr.Type;
	barr.Flags;
	cmdList->ResourceBarrier(1, &barr);
}

void DXContext::EndFrame()
{
}

void DXContext::Preset()
{
}
