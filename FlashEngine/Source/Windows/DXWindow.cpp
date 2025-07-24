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
		monitorInfo.rcWork.left + 10,
		monitorInfo.rcWork.top + 10,
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

void DXWindow::Resize() {
	DXContext::Get().ReleaseBuffers();
	RECT cr{};
	if (GetClientRect(m_hWnd, &cr))
	{
		m_Width = cr.right - cr.left;
		m_Height = cr.bottom - cr.top;

		DXContext::Get().GetSwapChain()->ResizeBuffers(DXContext::GetFrameCount(), m_Width, m_Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		m_ShouldResize = false;
	}
	DXContext::Get().GetBuffers();
}

void DXWindow::Shutdown()
{
}

void DXWindow::SetFullscreen(bool enabled)
{
	// Update window styling
	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
	if (enabled)
	{
		style = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
	}
	SetWindowLongW(DXWindow::Get().GetHWND(), GWL_STYLE, style);
	SetWindowLongW(DXWindow::Get().GetHWND(), GWL_EXSTYLE, exStyle);

	if (enabled)
	{
		HMONITOR monitor = MonitorFromWindow(DXWindow::Get().GetHWND(), MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (GetMonitorInfoW(monitor, &monitorInfo))
		{
			SetWindowPos(DXWindow::Get().GetHWND(), nullptr,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER
			);
		}
	}
	else
	{
		ShowWindow(DXWindow::Get().GetHWND(), SW_MAXIMIZE);
	}
	m_FullWindow = enabled;
}



LRESULT DXWindow::OnWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		if (lParam && (HIWORD(lParam) != Get().m_Height || LOWORD(lParam) != Get().m_Width))
		{
			Get().m_ShouldResize = true;
		}
		break;
	case WM_CLOSE :
		Get().m_ShouldClose = true;
		return 0;
	case WM_KEYDOWN:
		if (VK_F11 == wParam)
		{
			Get().SetFullscreen( !Get().GetFullWindow() );
		}
		else if(VK_ESCAPE == wParam)
		{
			if (Get().GetFullWindow())
			{
				Get().SetFullscreen(!Get().GetFullWindow());
			}
			else
			{
				Get().m_ShouldClose = true;
				return 0;
			}
		}
		break;
	default:
		break;
	}
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
