#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

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

private:
	// PSO
	bool CreatePSO(ComPointer<ID3D12Resource>& p_VBO, ComPointer<ID3D12RootSignature>& p_RootSignature, ComPointer<ID3D12PipelineState>& p_PipeState, D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps);
	bool CreateBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen, void* m_Data, D3D12_RESOURCE_STATES p_StateAfter);
	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature);
public:
	void CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char* p_Target, D3D12_SHADER_BYTECODE* p_Shader);

	BOOL InitShader(
		ComPointer<ID3D12RootSignature>& p_RootSignature, 
		ComPointer<ID3D12Resource>& p_VBO, 
		int p_DataLen, void* p_Data, 
		D3D12_RESOURCE_STATES p_StateAfter,
		ComPointer<ID3D12PipelineState>& p_PipeState, 
		D3D12_SHADER_BYTECODE p_vs, 
		D3D12_SHADER_BYTECODE p_ps
	);

private:
	D3DShader() = default;
};

 