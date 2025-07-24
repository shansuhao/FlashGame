#include "FlashExample.h"

int main(int argc, char* argv) {

#ifdef _DEBUG
	DXDebugLayer::Get().Init();
#endif // _DEBUG

	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT) && DXContext::Get().Init())
	{
		//float vertexData[] = {
		//	-0.5f, -0.5f, 0.5f, 1.0f, // position
		//	 1.0f,  0.0f, 0.0f, 1.0f,
		//	 0.0f,  0.0f, 0.0f, 0.0f,
		//	-0.5f, -0.5f, 0.5f, 1.0f, // position
		//	 0.5f,  0.5f, 0.5f, 1.0f,
		//	 0.0f,  0.0f, 0.0f, 0.0f,
		//	-0.5f, -0.5f, 0.5f, 1.0f, // position
		//	 0.0f,  0.0f, 1.0f, 1.0f,
		//	 0.0f,  0.0f, 0.0f, 0.0f,
		//};
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

			DXContext::Get().EndFrame();
			DXContext::Get().ExeuteCommandList();
			DXContext::Get().Preset();
		}
		DXContext::Get().Shutdown();
		DXWindow::Get().Shutdown();
	}
	return 0;
}