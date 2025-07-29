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
		staticMesh.SetVertexCount(3);
		staticMesh.SetVertexPosition(0, Vector4d(-1.0f, -1.0f, 0.5f, 1.0f));
		staticMesh.SetVertexTexcoord(0, Vector4d(1.0f, 0.0f, 0.0f, 1.0f));
		staticMesh.SetVertexNomal(0, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));
		staticMesh.SetVertexTangent(0, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));

		staticMesh.SetVertexPosition(1, Vector4d(0.0f, 1.0f, 0.5f, 1.0f));
		staticMesh.SetVertexTexcoord(1, Vector4d(0.0f, 1.0f, 0.0f, 1.0f));
		staticMesh.SetVertexNomal(1, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));
		staticMesh.SetVertexTangent(1, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));

		staticMesh.SetVertexPosition(2, Vector4d(1.0f, -1.0f, 0.5f, 1.0f));
		staticMesh.SetVertexTexcoord(2, Vector4d(0.0f, 0.0f, 1.0f, 1.0f));
		staticMesh.SetVertexNomal(2, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));
		staticMesh.SetVertexTangent(2, Vector4d(0.0f, 0.0f, 0.0f, 0.0f));

		ComPointer<ID3D12PipelineState> m_PipeState;
		ComPointer<ID3D12RootSignature> m_RootSignature;

		int BufferSize = sizeof(StaticMeshComponentVertexData) * staticMesh.m_VertexCount;

		D3D12_SHADER_BYTECODE t_vs = {};
		D3D12_SHADER_BYTECODE t_ps = {};
		D3D12_SHADER_BYTECODE t_RootSignature = {};

		D3DShader::Get().InitShaderFile(L"VertexShader.cso", &t_vs);
		D3DShader::Get().InitShaderFile(L"PixelShader.cso", &t_ps);
		D3DShader::Get().InitShaderFile(L"RootSignature.cso", &t_RootSignature); 
		//D3DShader::Get().CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainVS", "vs_5_0", &t_vs);
		//D3DShader::Get().CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainPS", "ps_5_0", &t_ps);

		BOOL p_IsInitShader_Success = false;
		p_IsInitShader_Success = D3DShader::Get().InitShader(
			m_RootSignature, staticMesh.m_VBO, 
			BufferSize, staticMesh.m_VertexData,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			m_PipeState, t_vs, t_ps, true, t_RootSignature
		);

		staticMesh.CreateVBOView();

		D3D12_VERTEX_BUFFER_VIEW vbos[] = {
			staticMesh.m_VBOView
		};

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
				DXContext::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				DXContext::Get().GetCommandList()->IASetVertexBuffers(0, 1, vbos);
				DXContext::Get().GetCommandList()->DrawInstanced(3, 1, 0, 0);
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