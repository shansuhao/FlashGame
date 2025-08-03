#pragma once
#include "stbi/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

namespace Flash {
	namespace ReadFile {
		void ReadImage(const char* p_fileName, int* p_ImageWidth, int* p_ImageHeight, int* p_imageChannel, stbi_uc** data);
	}
}