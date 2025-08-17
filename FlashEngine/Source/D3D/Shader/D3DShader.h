#pragma once

#include "Material.h"
#include "SceneNode.h"
#include "Camera/Camera.h"

#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"
#include "D3D/Mesh/StaticMeshComponent.h"

struct Texture2D {
	ComPointer<ID3D12Resource> mResource;
	DXGI_FORMAT mFormat;
};
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

	SceneNode* m_SphereNode = NULL;
private:
	float color[4] = { 0.5f, 0.5f, 0.5f, 1.f };

	D3D12_SHADER_BYTECODE m_vs = {};
	D3D12_SHADER_BYTECODE m_ps = {};
	D3D12_SHADER_BYTECODE m_gs = {};
	D3D12_SHADER_BYTECODE m_rs = {};

	ComPointer<ID3D12RootSignature> m_RootSignature;

public:
	DirectX::XMMATRIX m_ProjectionMatrix;

	Camera m_MainCamera;
public:
	// PSO
	bool CreatePSO(ComPointer<ID3D12RootSignature>& p_RootSignature, ComPointer<ID3D12PipelineState>& p_PipeState, D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs);

	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature);
	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature, D3D12_SHADER_BYTECODE p_RS);

	void CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char* p_Target, D3D12_SHADER_BYTECODE* p_Shader);
	void InitShaderFile(LPCTSTR p_ShaderFilePath, D3D12_SHADER_BYTECODE* p_Shader);

	Texture2D* LoadTexture2DFromFile(const char* inFilePath, ComPointer<ID3D12Resource>& texture);

	BOOL InitShader(BOOL isFromRootSignatureFile = false);

	bool InitRender();
	void Rendering();

	void Shutdown();
private:
	D3DShader() = default;
};

 