#include "pch.h"

#include "Logger.h" 

void Logger::PrintLog(unsigned int target, unsigned int type)
{
	glGetShaderiv(target, type, &success);
	if (!success)
	{
		glGetShaderInfoLog(target, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}