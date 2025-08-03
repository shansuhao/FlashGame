#include "FlashExample.h"
#include "Utils/ReadFile.h"
#include "D3D/Shader/D3DShader.h"
#include "D3D/Mesh/StaticMeshComponent.h"

int main(int argc, char* argv) {

#ifdef _DEBUG
	DXDebugLayer::Get().Init();
#endif // _DEBUG

	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT) && DXContext::Get().Init())
	{
		StaticMeshComponent staticMesh;

		D3D12_SHADER_BYTECODE t_vs = {};
		D3D12_SHADER_BYTECODE t_ps = {};
		D3D12_SHADER_BYTECODE t_gs = {};
		D3D12_SHADER_BYTECODE t_RootSignature = {};

		//D3DShader::Get().InitShaderFile(L"VertexShader.cso", &t_vs);
		//D3DShader::Get().InitShaderFile(L"PixelShader.cso", &t_ps);
		//D3DShader::Get().InitShaderFile(L"RootSignature.cso", &t_RootSignature); 
		D3DShader::Get().CreateShaderFromFile(L"Shaders/gs.hlsl", "MainVS", "vs_5_0", &t_vs);
		D3DShader::Get().CreateShaderFromFile(L"Shaders/gs.hlsl", "MainGS", "gs_5_0", &t_gs);
		D3DShader::Get().CreateShaderFromFile(L"Shaders/gs.hlsl", "MainPS", "ps_5_0", &t_ps);

		BOOL p_IsInitShader_Success = false;

		ComPointer<ID3D12PipelineState> m_PipeState;
		ComPointer<ID3D12RootSignature> m_RootSignature;

		DXContext::Get().InitCommandList();
		p_IsInitShader_Success = staticMesh.InitFromFile("Resource/Model/Sphere.lhsm");

		p_IsInitShader_Success = D3DShader::Get().InitShader(
			m_RootSignature, m_PipeState, t_vs, t_ps, t_gs, false, t_RootSignature
		);

		p_IsInitShader_Success = D3DShader::Get().CreateConstantBufferOBject(staticMesh.m_CB, 65536);
		DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
			(45.0f * 3.1415926f) / 180.0f,
			((float)DXWindow::Get().GetWidth() / (float)DXWindow::Get().GetHeight()),
			0.1f, 1000.0f
		);
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.f);
		modelMatrix *= DirectX::XMMatrixRotationZ(90.0f*3.1415926f/180.0f);
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
		D3DShader::Get().UpdateConstantBuffer(staticMesh.m_CB, matrix, sizeof(float) * 64);

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
		Flash::ReadFile::ReadImage("Resource/Image/Mario.png", &imageWidth, &imageHeight, &imageChannel, &data);

		ComPointer<ID3D12Resource> texture;
		p_IsInitShader_Success = D3DShader::Get().CreateTexture2D(texture, data, imageWidth * imageHeight * imageChannel, imageWidth, imageHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

		ComPointer<ID3D12Resource> texturePartice;
		p_IsInitShader_Success = D3DShader::Get().CreateTexture2D(texturePartice, particlePixels, 256 * 256 * 4, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
		delete[] particlePixels;
		delete data;

		/*******************************************************************************************************/
		ID3D12DescriptorHeap* srvHeap = NULL;
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.NumDescriptors = 2;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DXContext::Get().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));

		ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap };

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHeapPtr = srvHeap->GetCPUDescriptorHandleForHeapStart();
		DXContext::Get().GetDevice()->CreateShaderResourceView(texture, &srvDesc, srvHeapPtr);
		srvHeapPtr.ptr += DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		DXContext::Get().GetDevice()->CreateShaderResourceView(texturePartice, &srvDesc, srvHeapPtr);
		/*******************************************************************************************************/


		DXContext::Get().ExeuteCommandList();
		
		float color[] = {0.5f, 0.5f, 0.5f, 1.f};

		DWORD last_time = timeGetTime();
		DWORD appStartTime = last_time;
		DXWindow::Get().SetFullscreen(true);
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().UpdateWindow();

			DWORD current_time = timeGetTime();
			DWORD frameTime = current_time - last_time;
			DWORD timeSinceAppStartInMS = current_time - appStartTime;
			last_time = current_time;
			float deltaTimeInSecond = float(frameTime) / 1000.0f;
			float timeSinceAppStartInSecond = float(timeSinceAppStartInMS) / 1000.0f;

			color[0] = timeSinceAppStartInSecond;

			if (DXWindow::Get().ShouldResize())
			{
				DXContext::Get().Flush();
				DXWindow::Get().Resize();
			}
			// 对窗口进行渲染
			DXContext::Get().InitCommandList();
			DXContext::Get().DrawFrame();

			// 渲染三角形
			if (p_IsInitShader_Success)
			{
				DXContext::Get().GetCommandList()->SetPipelineState(m_PipeState);
				DXContext::Get().GetCommandList()->SetGraphicsRootSignature(m_RootSignature);
				DXContext::Get().GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
				DXContext::Get().GetCommandList()->SetGraphicsRootConstantBufferView(1, staticMesh.m_CB->GetGPUVirtualAddress());
				DXContext::Get().GetCommandList()->SetGraphicsRoot32BitConstants(0, 4, color, 0);
				DXContext::Get().GetCommandList()->SetGraphicsRootDescriptorTable(2, srvHeap->GetGPUDescriptorHandleForHeapStart());
				DXContext::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
				D3D12_VERTEX_BUFFER_VIEW vbos[] = {
					staticMesh.m_VBOView
				};
				DXContext::Get().GetCommandList()->IASetVertexBuffers(0, 1, vbos);
				DXContext::Get().GetCommandList()->DrawInstanced(staticMesh.m_VertexCount, 1, 0, 0);

				//staticMesh.Render();
			}

			DXContext::Get().EndFrame();
			DXContext::Get().ExeuteCommandList();
			DXContext::Get().Preset();
		}
		DXContext::Get().Shutdown();
		DXWindow::Get().Shutdown();
	}
	return 0;
}