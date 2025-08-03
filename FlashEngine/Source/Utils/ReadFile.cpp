#include "pch.h"
#include "ReadFile.h"

void Flash::ReadFile::ReadImage(const char* p_fileName, int* p_ImageWidth, int* p_ImageHeight, stbi_uc** data)
{
	FILE* pFile = nullptr;
	fopen_s(&pFile, p_fileName, "rb");
	if (pFile != nullptr)
	{
		int imageChannel;
		*data = stbi_load_from_file(pFile, p_ImageWidth, p_ImageHeight, &imageChannel, 4);
		fclose(pFile);
	}
}
