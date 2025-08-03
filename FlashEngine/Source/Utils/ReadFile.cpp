#include "pch.h"
#include "ReadFile.h"

void Flash::ReadFile::ReadImage(const char* p_fileName, int* p_ImageWidth, int* p_ImageHeight, int* p_imageChannel, stbi_uc** data)
{
	FILE* pFile = nullptr;
	fopen_s(&pFile, p_fileName, "rb");
	if (pFile != nullptr)
	{
		//stbi_set_flip_vertically_on_load(true);
		*data = stbi_load_from_file(pFile, p_ImageWidth, p_ImageHeight, p_imageChannel, 4);
		fclose(pFile);
	}
}
