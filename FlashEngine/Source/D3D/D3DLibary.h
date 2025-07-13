#pragma once

#define NOMINMAX

#include <wrl.h>
#include <iostream>
#include <Windows.h>
#include <wincodec.h>

using namespace Microsoft;
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifndef __VERIFY_EXPR
#define __VERIFY_EXPR(expr) do { if(FAILED(expr)) { return false; } } while(false)
#endif // !__VERIFY_EXPR

