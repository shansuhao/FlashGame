#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

struct FVector {
	float R, G, B = 0.f;

	float* GetRGB() const{
		float value[] = { R,G,B };
		return value;
	}
};

struct FVector4d
{
	float R, G, B, A = 0.f;
};

class DXContext
{
public:
	bool Init();
	void Shutdown();
	void ReleaseBuffers();
	bool CreateSwapChain();
	bool CreateDepthSource();
	void CreateRTVHeap();
	void CreateDSVHeap();
	bool GetBuffers();

	void SignalAndWait();

	void DrawFrame();
	void EndFrame();
	void Preset();

	void InitCommandList();
	void ExeuteCommandList();

	static constexpr size_t FrameCount = 2;
	static constexpr size_t GetFrameCount(){
		return FrameCount;
	}

	inline ComPointer<ID3D12GraphicsCommandList7>& GetCommandList() { return m_cmdList; }
	inline ComPointer<ID3D12Device10>& GetDevice() { return m_d3dDevice; }
	inline ComPointer<ID3D12CommandQueue>& GetCmdQueue() { return m_cmdQueue; }
	inline ComPointer<IDXGIFactory7>& GetFactory() { return m_dxgiFactory; }
	inline ComPointer<IDXGISwapChain3>& GetSwapChain() { return m_swapChain; }

	inline void Flush() {
		for (size_t i = 0; i < FrameCount; i++)
		{
			SignalAndWait();
		}
	}
private:
	ComPointer<IDXGIFactory7> m_dxgiFactory;
	ComPointer<ID3D12Device10> m_d3dDevice;

	ComPointer<ID3D12CommandAllocator> m_cmdAllocator;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList;

	ComPointer<ID3D12Fence1> m_d3dFence;
	HANDLE m_fenceEvent = nullptr;
	UINT64 m_fenceValue = 0;

	ComPointer<IDXGISwapChain3> m_swapChain;
	ComPointer<ID3D12Resource> m_d3d_ds_resoucre;
	ComPointer<ID3D12Resource> m_buffers[FrameCount];
	size_t m_currentBufferIndex = 0;

	ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];
	ComPointer<ID3D12DescriptorHeap> m_dsvDescHeap;

	FVector4d m_backGroundColor{ 0.3f,0.6f,0.9f,1.f };
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

