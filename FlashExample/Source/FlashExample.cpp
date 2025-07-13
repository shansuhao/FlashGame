#include "FlashExample.h"

int main(int argc, char* argv) {

#ifdef _DEBUG
	DXDebugLayer::Get().Init();
#endif // _DEBUG

	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT) && DXContext::Get().Init())
	{
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().UpdateWindow();

			// 对窗口进行渲染
			auto* cmdList = DXContext::Get().InitCommandList();
			DXContext::Get().DrawFrame(cmdList);

			DXContext::Get().EndFrame();
			DXContext::Get().Preset();
		}
		DXContext::Get().Shutdown();
		DXWindow::Get().Shutdown();
	}
	return 0;
}