#include "FlashExample.h"
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
		D3D12_SHADER_BYTECODE t_RootSignature = {};

		//D3DShader::Get().InitShaderFile(L"VertexShader.cso", &t_vs);
		//D3DShader::Get().InitShaderFile(L"PixelShader.cso", &t_ps);
		//D3DShader::Get().InitShaderFile(L"RootSignature.cso", &t_RootSignature); 
		D3DShader::Get().CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainVS", "vs_5_0", &t_vs);
		D3DShader::Get().CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainPS", "ps_5_0", &t_ps);

		BOOL p_IsInitShader_Success = false;

		ComPointer<ID3D12PipelineState> m_PipeState;
		ComPointer<ID3D12RootSignature> m_RootSignature;

		DXContext::Get().InitCommandList();
		p_IsInitShader_Success = staticMesh.InitFromFile("Resource/Model/Sphere.lhsm");

		p_IsInitShader_Success = D3DShader::Get().InitShader(
			m_RootSignature, m_PipeState, t_vs, t_ps, false, t_RootSignature
		);

		p_IsInitShader_Success = D3DShader::Get().CreateConstantBufferOBject(staticMesh.m_CB, 65536);
		DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
			(45.0f * 3.1415926f) / 180.0f,
			((float)DXWindow::Get().GetWidth() / (float)DXWindow::Get().GetHeight()),
			0.1f, 1000.0f
		);
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 10.f);
		DirectX::XMFLOAT4X4 tempMatrix;
		float matrix[48];
		DirectX::XMStoreFloat4x4(&tempMatrix, projectionMatrix);
		memcpy(matrix, &tempMatrix, sizeof(float) * 16);
		DirectX::XMStoreFloat4x4(&tempMatrix, viewMatrix);
		memcpy(matrix + 16, &tempMatrix, sizeof(float) * 16);
		DirectX::XMStoreFloat4x4(&tempMatrix, modelMatrix);
		memcpy(matrix + 32, &tempMatrix, sizeof(float) * 16);
		D3DShader::Get().UpdateConstantBuffer(staticMesh.m_CB, matrix, sizeof(float) * 48);


		DXContext::Get().ExeuteCommandList();
		

		float color[] = {0.5f, 0.5f, 0.5f, 1.f};

		DXWindow::Get().SetFullscreen(true);
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().UpdateWindow();
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
				DXContext::Get().GetCommandList()->SetGraphicsRoot32BitConstants(0, 4, color, 0);
				DXContext::Get().GetCommandList()->SetGraphicsRootConstantBufferView(1, staticMesh.m_CB->GetGPUVirtualAddress());
				DXContext::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				staticMesh.Render();
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