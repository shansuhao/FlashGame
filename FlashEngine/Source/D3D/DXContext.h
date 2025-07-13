#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

class DXContext
{
public:
	bool Init();
	void Shutdown();
	void ReleaseBuffers();
	bool CreateSwapChain();
	void CreateRTVHeap();
	bool GetBuffers();

	void DrawFrame(ID3D12GraphicsCommandList7* cmdList);
	void EndFrame();
	void Preset();

	ID3D12GraphicsCommandList7* InitCommandList();
	void ExeuteCommandList();

	static constexpr size_t FrameCount = 2;
	static constexpr size_t GetFramCount(){
		return FrameCount;
	}

	inline ComPointer<ID3D12CommandQueue>& GetCmdQueue() { return m_cmdQueue; }
	inline ComPointer<IDXGIFactory7>& GetFactory() { return m_dxgiFactory; }
private:
	ComPointer<IDXGIFactory7> m_dxgiFactory;
	ComPointer<ID3D12Device10> m_d3dDevice;

	ComPointer<ID3D12CommandAllocator> m_cmdAllocator;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList;

	ComPointer<ID3D12Fence1> m_d3dFence;
	HANDLE m_fenceEvent = nullptr;

	ComPointer<IDXGISwapChain3> m_swapChain;
	ComPointer<ID3D12Resource2> m_buffers[FrameCount];
	size_t m_currentBufferIndex = 0;

	ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;
	static DXContext& Get() {
		static DXContext instance;
		return instance;
	}

private:
	DXContext() = default;
};

