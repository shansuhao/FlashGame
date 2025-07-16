#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Log/D3DLog.h"

class OpenGLShader
{
public:
	unsigned int ID;

	OpenGLShader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
private:
	void CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
};

