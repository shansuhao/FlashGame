#include "FlashExample.h"

int main(int argc, char* argv) {
	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, WND_WIDTH, WND_HEIGHT))
	{
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().UpdateWindow();
		}
		DXWindow::Get().Shutdown();
	}
	return 0;
}