#include "pch.h"
#include "DXWindow.h"
#include "D3D/DXContext.h"

bool DXWindow::Init(LPCWSTR p_ClassName, LPCWSTR p_WndName, int32_t p_icon, UINT p_width, UINT p_height)
{
	WNDCLASSEXW wcex{};
	
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = p_ClassName;
	wcex.lpfnWndProc = OnWindowMessage;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(p_icon));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(p_icon));

	m_WndClass = RegisterClassExW(&wcex);
	if (m_WndClass == 0) return false;

	POINT pos{ 0,0 };
	GetCursorPos(&pos);
	HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfoW(monitor, &monitorInfo);

	m_hWnd = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		p_ClassName,
		p_ClassName,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		monitorInfo.rcWork.left + 100,
		monitorInfo.rcWork.top + 100,
		m_Width = p_width,
		m_Height = p_height,
		NULL, NULL, hInstance, NULL
	);
	if (m_hWnd == NULL) return false;

	return true;
}

void DXWindow::UpdateWindow()
{
	MSG msg{};
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void DXWindow::Shutdown()
{
}



LRESULT DXWindow::OnWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE :
		Get().m_ShouldClose = true;
		return 0;
	default:
		break;
	}
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
