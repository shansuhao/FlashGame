#include "pch.h"

#include <GL/glew.h>
#include <GL/GL.h>

#include "Log/Logger.h"

#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>

#include "OpenGLShader.h"

#pragma comment(lib, "opengl32.lib")

OpenGLShader::OpenGLShader(const char* vertexPath, const char* fragmentPath)
{
	//从文件中读取着色器文件 GLSL
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		vShaderFile.close();
		fShaderFile.close();
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	CompileShader(vShaderCode, fShaderCode);
}

// 编译顶点着色器与片段着色器(像素着色器)
// 顶点着色器用来绘制图形， 片段着色器用来上色
void OpenGLShader::CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
	unsigned int vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	Logger::Get().PrintLog(vertexShader, GL_COMPILE_STATUS);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	Logger::Get().PrintLog(fragmentShader, GL_COMPILE_STATUS);

	// 创建着色器程序，绑定着色器，并完成链接
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	Logger::Get().PrintLog(ID, GL_LINK_STATUS);

	glUseProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void OpenGLShader::use()
{
	glUseProgram(ID);
}

void OpenGLShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void OpenGLShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void OpenGLShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void OpenGLShader::setVec4(const std::string& name, Color color)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), color.R, color.G, color.B, color.A);
}
