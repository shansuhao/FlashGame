#include "pch.h"
#include "D3DShader.h"
#include "D3D/DXContext.h"
#include "Windows/DXWindow.h"

#include <filesystem>
#include <fstream>
#include <cstdlib>

// 参数设置结合宏定义处理
bool D3DShader::CreatePSO(ComPointer<ID3D12RootSignature>& p_RootSignature, ComPointer<ID3D12PipelineState>& p_PipeState,D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs)
{
	D3D12_INPUT_ELEMENT_DESC vertexElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 4, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float) * 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_LAYOUT_DESC vertexLayoutDesc = {};
	vertexLayoutDesc.NumElements = 4;
	vertexLayoutDesc.pInputElementDescs = vertexElementDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = p_RootSignature;
	psoDesc.VS = p_vs;
	psoDesc.PS = p_ps;
	psoDesc.GS = p_gs;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.InputLayout = vertexLayoutDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.DepthClipEnable = true;

	psoDesc.DepthStencilState.DepthEnable = true;
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

	__VERIFY_EXPR(DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&p_PipeState)));
	return true;
}

bool D3DShader::CreateBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen, void* m_Data, D3D12_RESOURCE_STATES p_StateAfter)
{
	D3D12_HEAP_PROPERTIES HeapProperties{};
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC d3d12_Resource_desc{};
	d3d12_Resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d12_Resource_desc.Alignment = 0;
	d3d12_Resource_desc.Width = p_DataLen;
	d3d12_Resource_desc.Height = 1;
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
		IID_PPV_ARGS(&p_VBO)
	));
	d3d12_Resource_desc = p_VBO->GetDesc();
	UINT64 memorySizeUsed = 0;
	UINT64 rowSizeInBytes = 0;
	UINT rowUsed = 0;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT subresourceFootprint;
	DXContext::Get().GetDevice()->GetCopyableFootprints(&d3d12_Resource_desc, 0, 1, 0, &subresourceFootprint, &rowUsed, &rowSizeInBytes, &memorySizeUsed);
	
	ID3D12Resource* tempBufferObject;
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
	tempBufferObject->Map(0, NULL, reinterpret_cast<void**>(&pData));
	BYTE* pDstTempBuffer = reinterpret_cast<BYTE*>(pData + subresourceFootprint.Offset);
	const BYTE* pSrcData = reinterpret_cast<BYTE*>(m_Data);
	for (size_t i = 0; i < rowUsed; i++)
	{
		memcpy(pDstTempBuffer + subresourceFootprint.Footprint.RowPitch * i, pSrcData + rowSizeInBytes * i, rowSizeInBytes);
	}
	tempBufferObject->Unmap(0, NULL);

	DXContext::Get().GetCommandList()->CopyBufferRegion(p_VBO, 0, tempBufferObject, 0, subresourceFootprint.Footprint.Width);
	InitResourceBarrier(p_VBO, D3D12_RESOURCE_STATE_COPY_DEST, p_StateAfter);
	
	return true;
}

void D3DShader::InitResourceBarrier(
	ComPointer<ID3D12Resource>& inResource, D3D12_RESOURCE_STATES inPrevState,
	D3D12_RESOURCE_STATES inNextState)
{
	D3D12_RESOURCE_BARRIER d3d12ResourceBarrier;
	memset(&d3d12ResourceBarrier, 0, sizeof(d3d12ResourceBarrier));
	d3d12ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d12ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d12ResourceBarrier.Transition.pResource = inResource;
	d3d12ResourceBarrier.Transition.StateBefore = inPrevState;
	d3d12ResourceBarrier.Transition.StateAfter = inNextState;
	d3d12ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	DXContext::Get().GetCommandList()->ResourceBarrier(1, &d3d12ResourceBarrier);
}

bool D3DShader::CreateConstantBufferOBject(ComPointer<ID3D12Resource>& p_VBO, int p_DataLen)
{
	D3D12_HEAP_PROPERTIES HeapProperties{};
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC d3d12_Resource_desc{};
	d3d12_Resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d12_Resource_desc.Alignment = 0;
	d3d12_Resource_desc.Width = p_DataLen;
	d3d12_Resource_desc.Height = 1;
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
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&p_VBO)
	));
	return true;
}

void D3DShader::UpdateConstantBuffer(ComPointer<ID3D12Resource>& p_VBO, void* p_Data, int p_DataLen)
{
	D3D12_RANGE uploadRange = {0};
	unsigned char* pBUffer = nullptr;
	p_VBO->Map(0, &uploadRange, (void**)(&pBUffer));
	memcpy(pBUffer, p_Data, p_DataLen);
	p_VBO->Unmap(0, NULL);
}

void CreateShaderFromFile(
	LPCTSTR inShaderFilePath,
	const char* inMainFunctionName,
	const char* inTarget,//"vs_5_0","ps_5_0","vs_4_0"
	D3D12_SHADER_BYTECODE* inShader)
{
	ID3DBlob* shaderBuffer = nullptr;
	ID3DBlob* errorBuffer = nullptr;
	HRESULT hResult = D3DCompileFromFile(inShaderFilePath, nullptr, nullptr,
		inMainFunctionName, inTarget, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &shaderBuffer, &errorBuffer);
	if (FAILED(hResult)) {
		printf("CreateShaderFromFile error : [%s][%s]:[%s]\n", inMainFunctionName, inTarget, (char*)errorBuffer->GetBufferPointer());
		errorBuffer->Release();
		return;
	}
	inShader->pShaderBytecode = shaderBuffer->GetBufferPointer();
	inShader->BytecodeLength = shaderBuffer->GetBufferSize();
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

void D3DShader::InitShaderFile(LPCTSTR p_ShaderFilePath, D3D12_SHADER_BYTECODE* p_Shader)
{
	static std::filesystem::path shaderDir;

	if (shaderDir.empty())
	{
		char moduleFileName[MAX_PATH];
		GetModuleFileNameA(NULL, moduleFileName, MAX_PATH);

		shaderDir = moduleFileName;
		shaderDir.remove_filename();
	}
	std::filesystem::path shaderFullFile = shaderDir / L"Shader" / p_ShaderFilePath;
	//shaderDir = shaderDir.append(L"/Shader");
	//shaderDir = shaderDir.append(p_ShaderFilePath);
	LPCTSTR ShaderFullPath = shaderFullFile.c_str();
	
	ID3DBlob* shaderBuffer = nullptr;
	HRESULT hResult = D3DReadFileToBlob(ShaderFullPath, &shaderBuffer);
	if (FAILED(hResult))
	{
		return;
	}
	p_Shader->pShaderBytecode = shaderBuffer->GetBufferPointer();
	p_Shader->BytecodeLength = shaderBuffer->GetBufferSize();
}

bool D3DShader::InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature)
{
	D3D12_ROOT_PARAMETER rootParameter[2] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[0].Constants.RegisterSpace = 0;
	rootParameter[0].Constants.ShaderRegister = 0;
	rootParameter[0].Constants.Num32BitValues = 4;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].Constants.RegisterSpace = 0;
	rootParameter[1].Constants.ShaderRegister = 1;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(rootParameter);
	rsDesc.pParameters = rootParameter;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature;
	HRESULT hResult = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, NULL);
	__VERIFY_EXPR(hResult);

	hResult = DXContext::Get().GetDevice()->CreateRootSignature(
		0, signature->GetBufferPointer(), 
		signature->GetBufferSize(), 
		IID_PPV_ARGS(&p_RootSignature)
	);
	__VERIFY_EXPR(hResult);
	return true;
}

bool D3DShader::InitRootSignature(ComPointer<ID3D12RootSignature>& p_RootSignature, D3D12_SHADER_BYTECODE p_RS)
{
	HRESULT hResult = DXContext::Get().GetDevice()->CreateRootSignature(0, p_RS.pShaderBytecode, p_RS.BytecodeLength, IID_PPV_ARGS(&p_RootSignature));

	__VERIFY_EXPR(hResult);
	return true;
}

BOOL D3DShader::InitShader(
	ComPointer<ID3D12RootSignature>& p_RootSignature,
	ComPointer<ID3D12PipelineState>& p_PipeState,
	D3D12_SHADER_BYTECODE p_vs, D3D12_SHADER_BYTECODE p_ps, D3D12_SHADER_BYTECODE p_gs,
	BOOL isFromRootSignatureFile, D3D12_SHADER_BYTECODE p_RS)
{
	// 初始化根签名
	if (isFromRootSignatureFile)
	{
		if (!InitRootSignature(p_RootSignature, p_RS)) return false;
	}
	else
	{
		if (!InitRootSignature(p_RootSignature)) return false;
	}
	if (!CreatePSO(p_RootSignature, p_PipeState, p_vs, p_ps, p_gs)) return false;

	return true;
}