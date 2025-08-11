#include "pch.h"
#include "Utils.h"
#include <math.h>
#include <algorithm>

namespace Flash {
	float srandom()
	{
		float number = float(rand()) / float(RAND_MAX);
		number *= 2.0f;
		number -= 1.0f;
		return number;
	}

	string WCharToMByte(LPCWSTR lpcwszStr)
	{
		string str;
		DWORD dwMinSize = 0;
		LPSTR lpszStr = NULL;
		dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
		if (0 == dwMinSize)
		{
			return FALSE;
		}
		lpszStr = new char[dwMinSize];
		WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
		str = lpszStr;
		delete[] lpszStr;
		return str;
	}

	LPCTSTR StringToLPCTSTR(string str) {
	
#ifdef UNICODE
		size_t size = str.length();
		wchar_t* buffer = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
		buffer[size] = 0;
		return buffer;
#else
		return str.c_str();
#endif // UNICODE
	}

	string GetProjectDir() {
		char* buffer = NULL;

		// 获取项目的工作路径
		buffer = _getcwd(NULL, 0);
		if (buffer) {
			std::string path = buffer;
			free(buffer);
			return path;
		}

		return "";
	}
}