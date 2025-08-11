#pragma once
#include <string>
#include <Windows.h>

using namespace std;

namespace Flash {
	struct GlobalConstants {
		float mProjectionMatrix[16];
		float mViewMatrix[16];
		float mMisc[4];
	};

	float srandom();
	string WCharToMByte(LPCWSTR lpcwszStr);
	LPCTSTR StringToLPCTSTR(string str);

	string GetProjectDir();
}