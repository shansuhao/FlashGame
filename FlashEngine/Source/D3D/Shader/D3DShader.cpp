#include "pch.h"
#include "D3DShader.h"
#include "D3D/DXContext.h"
#include "Windows/DXWindow.h"

// 参数设置结合宏定义处理
bool D3DShader::InitShader(D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps)
{
	D3D12_INPUT_ELEMENT_DESC vertexElementDesc[] = {
		{"POSITION",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 4, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_LAYOUT_DESC vertexLayoutDesc = {};
	vertexLayoutDesc.NumElements = 3;
	vertexLayoutDesc.pInputElementDescs = vertexElementDesc;

	CreateBufferOBject();
	if (!InitRootSignature())
	{
		return true;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = p_vs;
	psoDesc.PS = p_ps;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.InputLayout = vertexLayoutDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.DepthClipEnable = true;
	//psoDesc.RasterizerState.FrontCounterClockwise = false;
	//psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	//psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	//psoDesc.RasterizerState.MultisampleEnable = false;
	//psoDesc.RasterizerState.AntialiasedLineEnable = false;
	//psoDesc.RasterizerState.ForcedSampleCount = 0;
	//psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

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

bool D3DShader::CreateBufferOBject()
{
	D3D12_HEAP_PROPERTIES HeapProperties{};
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC d3d12_Resource_desc{};
	d3d12_Resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d12_Resource_desc.Alignment = 0;
	d3d12_Resource_desc.Width = m_DataLen;
	d3d12_Resource_desc.Height = m_DataWidth;
	d3d12_Resource_desc.DepthOrArraySize = 1;
	d3d12_Resource_desc.MipLevels = 1;
	d3d12_Resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	d3d12_Resource_desc.SampleDesc.Count = 1;
	d3d12_Resource_desc.SampleDesc.Quality = 0;
	d3d12_Resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3d12_Resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	__VERIFY_EXPR(DXContext::Get().GetDevice()->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE,
		&d3d12_Resource_desc, 
		D3D12_RESOURCE_STATE_COPY_DEST,
		NULL, 
		IID_PPV_ARGS(&m_CommittedResource)
	));

	d3d12_Resource_desc = m_CommittedResource->GetDesc();
	UINT64 memorySizeUsed = 0;
	UINT64 rowSizeInBytes = 0;
	UINT rowUsed = 0;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT subFootprint;
	DXContext::Get().GetDevice()->GetCopyableFootprints(&d3d12_Resource_desc, 0, 1, 0, &subFootprint, &rowUsed, &rowSizeInBytes, &memorySizeUsed);
	// 3 x 4 x 4 = 48bytes,32bytes,24bytes + 24bytes
	ComPointer<ID3D12Resource> tempBufferObject;
	HeapProperties = {};
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	__VERIFY_EXPR(DXContext::Get().GetDevice()->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE, 
		&d3d12_Resource_desc, 
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, 
		IID_PPV_ARGS(&tempBufferObject)
	));

	BYTE* pData;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = m_DataLen;
	tempBufferObject->Map(0, NULL, reinterpret_cast<void**>(&pData));
	BYTE* pDstTempBuffer = reinterpret_cast<BYTE*>(pData + subFootprint.Offset);
	const BYTE* pSrcData = reinterpret_cast<BYTE*>(m_Data);
	for (size_t i = 0; i < rowUsed; i++)
	{
		memcpy(pDstTempBuffer + subFootprint.Footprint.RowPitch * i, pSrcData + rowSizeInBytes * i, rowSizeInBytes);
	}
	tempBufferObject->Unmap(0, NULL);

	DXContext::Get().InitCommandList();
	DXContext::Get().GetCommandList()->CopyBufferRegion(m_CommittedResource, 0, tempBufferObject, 0, subFootprint.Footprint.Width);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_CommittedResource;
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = m_StateBefore;
	barrier.Transition.StateAfter = m_StateAfter;
	DXContext::Get().GetCommandList()->ResourceBarrier(1, &barrier);

	DXContext::Get().ExeuteCommandList();

	return true;
}

void D3DShader::CreateShaderFromFile(LPCTSTR p_ShaderFilePath, const char* p_MainFunctionName, const char * p_Target, D3D12_SHADER_BYTECODE* p_Shader)
{
	ID3DBlob* shaderBuffer = nullptr;
	ID3DBlob* errorBuffer = nullptr;
	HRESULT hResult = D3DCompileFromFile(p_ShaderFilePath, nullptr, nullptr, p_MainFunctionName, p_Target, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shaderBuffer, &errorBuffer);

	if (FAILED(hResult))
	{
		printf("CreateShaderFromFile Error: [%s][%s]:[%s]\n", p_ShaderFilePath, p_Target, (char*)errorBuffer->GetBufferPointer());
		errorBuffer->Release();
		return;
	}
	p_Shader->pShaderBytecode = shaderBuffer->GetBufferPointer();
	p_Shader->BytecodeLength = shaderBuffer->GetBufferSize();
}

bool D3DShader::InitRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {}; 
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature;
	HRESULT hResult = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, NULL);
	__VERIFY_EXPR(hResult);
	hResult = DXContext::Get().GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	__VERIFY_EXPR(hResult);
	return true;
}

void D3DShader::ShutDown()
{
	m_PipeState.Release();
	m_RootSignature.Release();
	m_CommittedResource.Release();
}
