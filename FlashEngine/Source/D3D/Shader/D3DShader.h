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

public:
	// PSO
	bool CreatePSO(ComPointer<ID3D12RootSignature>& p_RootSignature, ComPointer<ID3D12PipelineState>& p_PipeState, D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs);
	void InitResourceBarrier(ComPointer<ID3D12Resource>& inResource, D3D12_RESOURCE_STATES inPrevState, D3D12_RESOURCE_STATES inNextState);
	//bool CreateBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen, void* m_Data, D3D12_RESOURCE_STATES p_StateAfter);
	bool CreateConstantBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen);
	void UpdateConstantBuffer(ComPointer<ID3D12Resource>& p_VBO, void* p_Data, int p_DataLen);
	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature);
	bool InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature, D3D12_SHADER_BYTECODE p_RS);

	void CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char* p_Target, D3D12_SHADER_BYTECODE* p_Shader);
	void InitShaderFile(LPCTSTR p_ShaderFilePath, D3D12_SHADER_BYTECODE* p_Shader);

	bool CreateBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen, void* m_Data, D3D12_RESOURCE_STATES p_StateAfter);

	BOOL InitShader(
		ComPointer<ID3D12RootSignature>& p_RootSignature,
		ComPointer<ID3D12PipelineState>& p_PipeState,
		D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs,
		BOOL isFromRootSignatureFile = false, D3D12_SHADER_BYTECODE p_RS = {}
	);

private:
	D3DShader() = default;
};

 