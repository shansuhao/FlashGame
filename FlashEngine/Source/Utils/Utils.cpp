#include "pch.h"
#include "Utils.h"
#include <math.h>
#include <algorithm>


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