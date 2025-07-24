#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

class D3DShader
{
public:
	D3DShader() = default;
	~D3DShader() = default;
private:
	ComPointer<ID3D12PipelineState> m_PipeState;
	ComPointer<ID3D12RootSignature> m_RootSignature;

	D3D12_SHADER_BYTECODE m_vs = {};
	D3D12_SHADER_BYTECODE m_ps = {};
public:
	// PSO
	bool CreatePSO();
	bool CreateVBO();
};

