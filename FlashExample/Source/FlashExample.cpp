#include "FlashExample.h"
#include "D3D/Shader/D3DShader.h"

int main(int argc, char* argv) {

#ifdef _DEBUG
	DXDebugLayer::Get().Init();
#endif // _DEBUG

	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT) && DXContext::Get().Init())
	{
		float vertexData[] = {
			-1.0f, -1.0f, 0.5f, 1.0f, // position
			 1.0f,  0.0f, 0.0f, 1.0f,
			 0.0f,  0.0f, 0.0f, 0.0f,
			 0.0f,  1.0f, 0.5f, 1.0f, // position
			 0.0f,  1.0f, 0.0f, 1.0f,
			 0.0f,  0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.5f, 1.0f, // position
			 0.0f,  0.0f, 1.0f, 1.0f,
			 0.0f,  0.0f, 0.0f, 0.0f,
		};

		D3DShader temp{ sizeof(vertexData), vertexData, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER };
		D3D12_SHADER_BYTECODE t_vs = {};
		D3D12_SHADER_BYTECODE t_ps = {};
		temp.CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainVS", "vs_5_0", &t_vs);
		temp.CreateShaderFromFile(L"Shaders/ndctriangle.hlsl", "MainPS", "ps_5_0", &t_ps);
		BOOL p_IsInitShader_Success = temp.InitShader(t_vs, t_ps);

		// 创建VBO
		D3D12_VERTEX_BUFFER_VIEW vboBufferView = {};
		vboBufferView.BufferLocation = temp.GetVBO().Get()->GetGPUVirtualAddress();
		vboBufferView.SizeInBytes = sizeof(vertexData);
		vboBufferView.StrideInBytes = sizeof(float) * 12;
		D3D12_VERTEX_BUFFER_VIEW vbos[] = {
			vboBufferView
		};

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
				DXContext::Get().GetCommandList()->SetPipelineState(temp.GetPSO());
				DXContext::Get().GetCommandList()->SetGraphicsRootSignature(temp.GetRootSignature());
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