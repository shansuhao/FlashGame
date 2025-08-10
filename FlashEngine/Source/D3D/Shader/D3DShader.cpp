#include "pch.h"
#include "D3DShader.h"
#include "Utils/Utils.h"
#include "Utils/ReadFile.h"
#include "D3D/DXContext.h"
#include "Windows/DXWindow.h"
#include "D3D/Mesh/StaticMeshComponent.h"

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
	psoDesc.pRootSignature = p_RootSignature.Get();
	psoDesc.VS = p_vs;
	psoDesc.PS = p_ps;
	psoDesc.GS = p_gs;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.InputLayout = vertexLayoutDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;

	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	psoDesc.BlendState = {0};
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {
		FALSE,FALSE,
		D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
		D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
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
		char szLog[1024] = { 0 };
		strcpy_s(szLog, (char*)errorBuffer->GetBufferPointer());
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
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].Constants.RegisterSpace = 0;
	rootParameter[0].Constants.ShaderRegister = 0;
	rootParameter[0].Constants.Num32BitValues = 4;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].Constants.RegisterSpace = 0;
	rootParameter[1].Constants.ShaderRegister = 1;

	D3D12_DESCRIPTOR_RANGE descRange[1];
	descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descRange[0].RegisterSpace = 0;
	descRange[0].BaseShaderRegister = 0;  //t0
	descRange[0].NumDescriptors = 1;
	descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descRange;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].Descriptor.RegisterSpace = 1;
	rootParameter[3].Descriptor.ShaderRegister = 0;

	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	memset(samplerDesc,0, sizeof(D3D12_STATIC_SAMPLER_DESC)*_countof(samplerDesc));
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].RegisterSpace = 0;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(rootParameter);
	rsDesc.pParameters = rootParameter;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumStaticSamplers = _countof(samplerDesc);
	rsDesc.pStaticSamplers = samplerDesc;

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

BOOL D3DShader::InitShader(BOOL isFromRootSignatureFile)
{
	// 初始化根签名
	if (isFromRootSignatureFile)
	{
		if (!InitRootSignature(m_RootSignature, m_rs)) return false;
	}
	else
	{
		if (!InitRootSignature(m_RootSignature)) return false;
	}
	if (!CreatePSO(m_RootSignature, m_PipeState, m_vs, m_ps, m_gs)) return false;

	return true;
}

bool D3DShader::InitRender(StaticMeshComponent* staticMesh)
{
	bool p_IsInitShader_Success = false;

	//D3DShader::Get().InitShaderFile(L"VertexShader.cso", &t_vs);
	//D3DShader::Get().InitShaderFile(L"PixelShader.cso", &t_ps);
	//D3DShader::Get().InitShaderFile(L"RootSignature.cso", &t_RootSignature); 

	LPWSTR CurrentPath = new WCHAR;
	LPCTSTR HLSLFile = new WCHAR;
	DWORD Result = GetCurrentDirectory(MAX_PATH, CurrentPath);
	if (Result != 0)
	{
		std::string str_CurrentPath = WCharToMByte(CurrentPath);
		str_CurrentPath = str_CurrentPath.append("/Shaders/gs.hlsl");
#ifdef UNICODE
		std::wstring wstr(str_CurrentPath.begin(), str_CurrentPath.end());
		HLSLFile = wstr.c_str();
#else
		HLSLFile = str_CurrentPath.c_str();
#endif
	}

	//DXContext::Get().InitCommandList();
	p_IsInitShader_Success = InitShader(false);

	p_IsInitShader_Success = DXContext::Get().CreateConstantBufferOBject(staticMesh->m_CB, 65536);

	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		(45.0f * 3.141592f) / 180.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.f);
	//modelMatrix *= DirectX::XMMatrixRotationY(90.0f*3.1415926f/180.0f);
	DirectX::XMFLOAT4X4 tempMatrix;
	float matrix[64];
	DirectX::XMStoreFloat4x4(&tempMatrix, projectionMatrix);
	memcpy(matrix, &tempMatrix, sizeof(float) * 16);
	DirectX::XMStoreFloat4x4(&tempMatrix, viewMatrix);
	memcpy(matrix + 16, &tempMatrix, sizeof(float) * 16);
	DirectX::XMStoreFloat4x4(&tempMatrix, modelMatrix);
	memcpy(matrix + 32, &tempMatrix, sizeof(float) * 16);
	DirectX::XMVECTOR determinant;
	DirectX::XMMATRIX inverseModelMatrix = DirectX::XMMatrixInverse(&determinant, modelMatrix);
	if (DirectX::XMVectorGetX(determinant) != 0.0f)
	{
		DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(inverseModelMatrix);
		DirectX::XMStoreFloat4x4(&tempMatrix, modelMatrix);
		memcpy(matrix + 48, &tempMatrix, sizeof(float) * 16);
	}
	DXContext::Get().UpdateConstantBuffer(staticMesh->m_CB, matrix, sizeof(float) * 64);

	p_IsInitShader_Success = DXContext::Get().CreateConstantBufferOBject(m_sb, 65536);
	struct MaterialData {
		float r;
	};
	MaterialData* materialDatas = new MaterialData[3000];
	for (int i = 0; i < 3000; i++)
	{
		materialDatas[i].r = srandom() * 0.1f + 0.1f;
	}
	DXContext::Get().UpdateConstantBuffer(m_sb, materialDatas, sizeof(MaterialData) * 3000);

	// 生成图片
	unsigned char* particlePixels = new unsigned char[256 * 256 * 4];
	memset(particlePixels, 0, 256 * 256 * 4);
	for (size_t y = 0; y < 256; y++)
	{
		for (size_t x = 0; x < 256; x++)
		{
			float radiusSqrt = float((x - 128) * (x - 128) + (y - 128) * (y - 128));
			if (radiusSqrt <= 128 * 128)
			{
				float radius = sqrtf(radiusSqrt);
				float alpha = radius / 128.0f;
				alpha = alpha > 1.0f ? 1.0f : alpha;
				alpha = 1.0f - alpha;
				alpha = powf(alpha, 2.0f);
				int pixelIndex = y * 256 + x;
				particlePixels[pixelIndex * 4] = 255;
				particlePixels[pixelIndex * 4 + 1] = 255;
				particlePixels[pixelIndex * 4 + 2] = 255;
				particlePixels[pixelIndex * 4 + 3] = unsigned char(alpha * 255);
			}
		}
	}

	stbi_uc* data = nullptr;
	int imageWidth, imageHeight, imageChannel;
	ComPointer<ID3D12Resource> texture, texturePartice;
	Flash::ReadFile::ReadImage("/Resource/Image/earth_d.jpg", &imageWidth, &imageHeight, &imageChannel, &data);
	p_IsInitShader_Success = DXContext::Get().CreateTexture2D(texture, data, imageWidth * imageHeight * imageChannel, imageWidth, imageHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	p_IsInitShader_Success = DXContext::Get().CreateTexture2D(texturePartice, particlePixels, 256 * 256 * 4, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
	delete[] particlePixels;
	delete data;

	/*******************************************************************************************************/
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXContext::Get().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

	/*******************************************************************************************************/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapPtr = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
	DXContext::Get().GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, srvHeapPtr);

	srvHeapPtr.ptr += DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DXContext::Get().GetDevice()->CreateShaderResourceView(texturePartice.Get(), &srvDesc, srvHeapPtr);
	/*******************************************************************************************************/
	D3D12_SHADER_RESOURCE_VIEW_DESC sbSRVDesc = {};
	sbSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sbSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	sbSRVDesc.Buffer.FirstElement = 0;
	sbSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	sbSRVDesc.Buffer.NumElements = 3000;
	sbSRVDesc.Buffer.StructureByteStride = sizeof(MaterialData);

	srvHeapPtr.ptr += DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DXContext::Get().GetDevice()->CreateShaderResourceView(m_sb.Get(), &sbSRVDesc, srvHeapPtr);
	DXContext::Get().ExeuteCommandList();
	return false;
}

void D3DShader::Rendering(StaticMeshComponent* staticMesh)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap };

	DXContext::Get().GetCommandList()->SetPipelineState(m_PipeState);
	DXContext::Get().GetCommandList()->SetGraphicsRootSignature(m_RootSignature);
	DXContext::Get().GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DXContext::Get().GetCommandList()->SetGraphicsRoot32BitConstants(0, 4, color, 0);
	DXContext::Get().GetCommandList()->SetGraphicsRootConstantBufferView(1, staticMesh->m_CB->GetGPUVirtualAddress());
	DXContext::Get().GetCommandList()->SetGraphicsRootDescriptorTable(2, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
	DXContext::Get().GetCommandList()->SetGraphicsRootShaderResourceView(3, m_sb->GetGPUVirtualAddress());
	DXContext::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//D3D12_VERTEX_BUFFER_VIEW vbos[] = {
	//	staticMesh.m_VBOView
	//};
	//DXContext::Get().GetCommandList()->IASetVertexBuffers(0, 1, vbos);
	//DXContext::Get().GetCommandList()->DrawInstanced(staticMesh.m_VertexCount, 1, 0, 0);

	staticMesh->Render();
}

void D3DShader::Shutdown() {

}