#include "FlashExample.h"
#include "Utils/Utils.h"
#include "Utils/ReadFile.h"
#include "D3D/Shader/D3DShader.h"
#include "D3D/Mesh/StaticMeshComponent.h"

int main(int argc, char* argv) {

#ifdef _DEBUG
	DXDebugLayer::Get().Init();
#endif // _DEBUG

	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT) && DXContext::Get().Init())
	{
		//DXWindow::Get().SetFullscreen(true);

		BOOL p_IsInitShader_Success = false;
		
		p_IsInitShader_Success = D3DShader::Get().InitRender();

		ShowWindow(DXWindow::Get().GetHWND(), SW_SHOWDEFAULT);
		UpdateWindow(DXWindow::Get().GetHWND());
		
		float color[] = {0.5f, 0.5f, 0.5f, 1.f};
		DWORD last_time = timeGetTime();
		DWORD appStartTime = last_time;

		while (!DXWindow::Get().ShouldClose())
		{
			if (!DXWindow::Get().UpdateWindow()){

				if (DXWindow::Get().ShouldResize())
				{
					DXContext::Get().Flush();
					DXWindow::Get().Resize();
				}

				// 对窗口进行渲染
				DXContext::Get().InitCommandList();
				DXContext::Get().DrawFrame();

				if (p_IsInitShader_Success)
				{
					DWORD current_time = timeGetTime();
					DWORD frameTime = current_time - last_time;
					DWORD timeSinceAppStartInMS = current_time - appStartTime;
					last_time = current_time;
					float deltaTimeInSecond = float(frameTime) / 1000.0f;
					float timeSinceAppStartInSecond = float(timeSinceAppStartInMS) / 1000.0f;
					D3DShader::Get().GetColor()[0] = timeSinceAppStartInSecond;

					D3DShader::Get().Rendering();
				}

				DXContext::Get().EndFrame();
				DXContext::Get().ExeuteCommandList();
				DXContext::Get().Preset();
			}
		}
		D3DShader::Get().Shutdown();
		DXContext::Get().Shutdown();
		DXWindow::Get().Shutdown();
	}
	return 0;
}