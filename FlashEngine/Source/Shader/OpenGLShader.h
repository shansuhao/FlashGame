#pragma once
struct Color {
	float R, G, B, A = 0;
};
class OpenGLShader
{
public:
	unsigned int ID = 0;

	OpenGLShader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec4(const std::string &name, Color color);
private:
	void CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
};

