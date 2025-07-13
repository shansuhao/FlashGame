#pragma once
#include "D3D/D3DLibary.h"
#include "Utils/ComPointer.h"

class DXWindow
{
private:
	ATOM		m_WndClass = 0;
	HINSTANCE	hInstance = GetModuleHandleW(NULL);
	
	HWND		m_hWnd = nullptr;
	UINT		m_Width = 1920;
	UINT		m_Height = 1080;
	BOOL		m_ShouldClose = false;
	BOOL		m_FullWindow = false;
public:
	bool Init(LPCWSTR p_ClassName, LPCWSTR p_WndName, int32_t p_icon, UINT p_width, UINT p_height);
	void UpdateWindow();
	void Shutdown();
	void SetFullscreen(bool enabled);


	inline bool ShouldClose() const { return m_ShouldClose; }
	inline HWND& GetHWND() { return m_hWnd; }
	inline UINT GetWidth() const { return m_Width; }
	inline UINT GetHeight() const { return m_Height; }
	inline BOOL GetFullWindow() { return m_FullWindow; }

private:
	static LRESULT CALLBACK OnWindowMessage(HWND, UINT, WPARAM, LPARAM);
public:
	inline static DXWindow& Get() {
		static DXWindow instance;
		return instance;
	}
public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;
private:
	DXWindow() = default;
};

