#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"
#include "D3D/Mesh/StaticMeshComponent.h"

/*
* 渲染：
*	后续拆分为三部分：网格体、材质、纹理
*/
class D3DShader
{
public:
	D3DShader(const D3DShader&) = delete;
	D3DShader& operator=(const D3DShader&) = delete;
	~D3DShader() = default;

	static D3DShader& Get() {
		static D3DShader self;
		return self;
	}
	inline float* GetColor() { return color; }

	inline D3D12_SHADER_BYTECODE* GetVS() { return &m_vs; }
	inline D3D12_SHADER_BYTECODE* GetPS() { return &m_ps; }
	inline D3D12_SHADER_BYTECODE* GetGS() { return &m_gs; }
	inline D3D12_SHADER_BYTECODE* GetRS() { return &m_rs; }

	inline ComPointer<ID3D12DescriptorHeap>& GetSRVHeap() { return m_srvHeap; }
	inline ComPointer<ID3D12Resource>& GetSB() { return m_sb; }
private:
	float color[4] = { 0.5f, 0.5f, 0.5f, 1.f };

	D3D12_SHADER_BYTECODE m_vs = {};
	D3D12_SHADER_BYTECODE m_ps = {};
	D3D12_SHADER_BYTECODE m_gs = {};
	D3D12_SHADER_BYTECODE m_rs = {};
	ComPointer<ID3D12DescriptorHeap> m_srvHeap;
	ComPointer<ID3D12PipelineState> m_PipeState;
	ComPointer<ID3D12RootSignature> m_RootSignature;
	ComPointer<ID3D12Resource> m_sb;

public:
	DirectX::XMMATRIX m_ProjectionMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
public:
	// PSO
	bool CreatePSO(ComPointer<ID3D12RootSignature>& p_RootSignature, ComPointer<ID3D12PipelineState>& p_PipeState, D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs);

	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature);
	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature, D3D12_SHADER_BYTECODE p_RS);

	void CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char* p_Target, D3D12_SHADER_BYTECODE* p_Shader);
	void InitShaderFile(LPCTSTR p_ShaderFilePath, D3D12_SHADER_BYTECODE* p_Shader);

	BOOL InitShader(BOOL isFromRootSignatureFile = false);

	bool InitRender(StaticMeshComponent* staticMesh);
	void Rendering(StaticMeshComponent* staticMesh);

	void Shutdown();
private:
	D3DShader() = default;
};

 