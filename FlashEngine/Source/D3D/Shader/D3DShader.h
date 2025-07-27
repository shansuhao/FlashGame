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
	D3DShader() = default;
	D3DShader(UINT p_dataLen, void* p_data, D3D12_RESOURCE_STATES  ):m_DataLen(p_dataLen), m_Data(p_data), m_StateAfter(m_StateAfter)
	{
	}

	~D3DShader() {
		ShutDown();
	}
private:
	ComPointer<ID3D12PipelineState> m_PipeState;
	ComPointer<ID3D12RootSignature> m_RootSignature;
	ComPointer<ID3D12Resource> m_CommittedResource;

	D3D12_RESOURCE_STATES m_StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	D3D12_RESOURCE_STATES m_StateAfter = {};

	UINT m_DataLen = 1024;		//数据长度--> D3D12_RESOURCE_DESC.Width
	UINT m_DataWidth = 1;	//数据宽度--> D3D12_RESOURCE_DESC.Height

	void* m_Data;
public:
	// PSO
	bool InitShader(D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps);
	bool CreateBufferOBject();
	void CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char* p_Target, D3D12_SHADER_BYTECODE* p_Shader);
	bool InitRootSignature();

	inline ComPointer<ID3D12PipelineState> GetPSO() const { return m_PipeState; }
	inline ComPointer<ID3D12RootSignature> GetRootSignature() const { return m_RootSignature; }
	inline ComPointer<ID3D12Resource> GetVBO() const { return m_CommittedResource; }
private:
	void ShutDown();
};

 