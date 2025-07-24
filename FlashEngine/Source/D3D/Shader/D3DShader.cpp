#include "pch.h"
#include "D3DShader.h"
#include "D3D/DXContext.h"

// 参数设置结合宏定义处理
bool D3DShader::CreatePSO()
{
	D3D12_INPUT_ELEMENT_DESC vertexElementDesc[] = {
		{"POSITIONT",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_LAYOUT_DESC vertexLayoutDesc = {};
	vertexLayoutDesc.NumElements = 3;
	vertexLayoutDesc.pInputElementDescs = vertexElementDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = m_vs;
	psoDesc.PS = m_ps;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.InputLayout = vertexLayoutDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//psoDesc.RasterizerState.FrontCounterClockwise = false;
	//psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	//psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.DepthClipEnable = true;
	//psoDesc.RasterizerState.MultisampleEnable = false;
	//psoDesc.RasterizerState.AntialiasedLineEnable = false;
	//psoDesc.RasterizerState.ForcedSampleCount = 0;
	//psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;

	psoDesc.BlendState = {0};
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {
		FALSE, FALSE, 
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	for (size_t i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		psoDesc.BlendState.RenderTarget[i] = rtBlendDesc;
	}
	psoDesc.NumRenderTargets = 1;

	__VERIFY_EXPR(DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipeState)));
	return true;
}

bool D3DShader::CreateVBO()
{
	return false;
}
