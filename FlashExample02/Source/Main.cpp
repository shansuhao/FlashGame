#include "FlashExample.h"

#include "OpenGL/OpenGlLibary.h"

#include "Shader/OpenGLShader.h"
#include "Log/Logger.h"

struct FColor
{
	float R, G, B, A = 0;
};

static FColor background = { 0.f, 0.8f, 0.8f, 0.f };
static float g_width = WND_WIDTH;
static float g_height = WND_HEIGHT;

const char* g_vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	vertexColor = vec4(aColor.xyz, 1.0);\n"
"}\0";

const char* g_fragmentShaderSource_1 = "#version 330 core \n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"void main()\n"
"{\n"
"	FragColor = vertexColor;\n"
"}\0";

const char* g_fragmentShaderSource_2 = "#version 330 core \n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"	FragColor = ourColor;\n"
"}\0";

float g_time = 0.f;

bool OpenGLInit(__in HWND p_hwnd, __out HDC& p_hdc, __out HGLRC& p_hglrc);
void UpdateWindow(HWND hwnd, HDC hdc, HGLRC hglrc);
void Render();
void RenderTriangle(unsigned int& VBO, unsigned int& VAO, float* vertices, size_t size);
void CompileShader(unsigned int& shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource);

int main(int argc, char* argv) {
	HDC hdc = {};
	if (DXWindow::Get().Init(WND_CLASS_NAME, WND_CLASS_NAME, IDI_ICON1, g_width, g_height))
	{
		HGLRC hglrc = {};
		if (!OpenGLInit(DXWindow::Get().GetHWND(), hdc, hglrc))
		{
			return 0;
		}

		OpenGLShader shader("./Shader/vertex.glsl","./Shader/fragment.glsl");

		//unsigned int shaderProgram_1, shaderProgram_2;

		//CompileShader(shaderProgram_1, g_vertexShaderSource, g_fragmentShaderSource_1);
		//CompileShader(shaderProgram_2, g_vertexShaderSource, g_fragmentShaderSource_2);

		//float vertices[] = {
		//-0.5f, -0.5f, 0.0f,
		// 0.5f, -0.5f, 0.0f,
		// 0.0f,  0.5f, 0.0f
		//};

		//// 绘制矩形 方案 1
		//float vertices[] = {
		//	// 第一个三角形
		//	0.5f, 0.5f, 0.0f,   // 右上角
		//	0.5f, -0.5f, 0.0f,  // 右下角
		//	-0.5f, 0.5f, 0.0f,  // 左上角
		//	// 第二个三角形
		//	0.5f, -0.5f, 0.0f,  // 右下角
		//	-0.5f, -0.5f, 0.0f, // 左下角
		//	-0.5f, 0.5f, 0.0f   // 左上角
		//};

		// 绘制矩形 方案2
		//float vertices[] = {
		//	 0.5f,  0.5f, 0.0f,   1.f,0.f,0.f,// 右上角
		//	 0.5f, -0.5f, 0.0f,	  0.f,1.f,0.f,// 右下角
		//	-0.5f, -0.5f, 0.0f,	  1.f,0.f,0.f,// 左下角
		//	-0.5f,  0.5f, 0.0f,   0.f,0.f,1.f,// 左上角
		//};

		float vertices[] = {
			 0.0f,  0.5f, 0.0f,   1.f,0.f,0.f,// 右上角
			 0.5f, -0.5f, 0.0f,	  0.f,1.f,0.f,// 右下角
			-0.5f, -0.5f, 0.0f,	  1.f,0.f,0.f,// 左下角
			-0.5f,  0.5f, 0.0f,   0.f,0.f,1.f,// 左上角
		};

		unsigned int indices[] = {
			// 注意索引从0开始! 
			// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
			// 这样可以由下标代表顶点组合成矩形

			0, 1, 2, // 第一个三角形
			//1, 2, 3  // 第二个三角形
		};

		unsigned int VBO, VAO, EBO;
		glGenBuffers(1,&EBO);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//float vertices_1[] = {
		//	0.5f, 0.5f, 0.0f,   // 右上角
		//	0.5f, -0.5f, 0.0f,  // 右下角
		//	-0.5f, 0.5f, 0.0f,  // 左上角
		//};

		//float vertices_2[] = {
		//	0.5f, -0.5f, 0.0f,  // 右下角
		//	-0.5f, -0.5f, 0.0f, // 左下角
		//	-0.5f, 0.5f, 0.0f   // 左上角
		//};

		//unsigned int VBO_1, VAO_1, VBO_2, VAO_2;

		//RenderTriangle(VBO_1, VAO_1, vertices_1, sizeof(vertices_1));
		//RenderTriangle(VBO_2, VAO_2, vertices_2, sizeof(vertices_2));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		std::cout << "Tick Count:" << GetTickCount() << std::endl;
		DXWindow::Get().SetFullscreen(TRUE);
		while (!DXWindow::Get().ShouldClose())
		{
			//time += (1 / GetTickCount());

			g_time += 0.001;
			DXWindow::Get().UpdateWindow();
			UpdateWindow(DXWindow::Get().GetHWND(), hdc, hglrc);

			//Render();

			//glUseProgram(shaderProgram_1);
			shader.use();
			shader.setVec4("ourColor", Color(abs(tan(g_time)), abs(cos(g_time)), abs(sin(g_time)), abs(sin(g_time))));
			shader.setInt("aForward", -1);
			// 绘制三角形
			glBindVertexArray(VAO);
			//// 绘制矩形 方案1
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			// 绘制矩形 方案2

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); glBindVertexArray(0);

			//glUseProgram(shaderProgram_2);

			//int vertexColorLocation = glGetUniformLocation(shaderProgram_2, "ourColor");
			//glUniform4f(vertexColorLocation, sin(g_time), cos(g_time), tan(g_time), sin(g_time));

			//glBindVertexArray(VAO_1);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			//glBindVertexArray(VAO_2);
			//glDrawArrays(GL_TRIANGLES, 0, 3);

			SwapBuffers(hdc);
		}

		DXWindow::Get().Shutdown();
	}
	return 0;
}

void RenderTriangle(unsigned int& VBO, unsigned int& VAO, float *vertices, size_t size) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool OpenGLInit(__in HWND p_hwnd, __out HDC& p_hdc, __out HGLRC& p_hglrc)
{
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	p_hdc = GetDC(p_hwnd);
	auto pixelFormat = ChoosePixelFormat(p_hdc, &pfd);
	if (!pixelFormat)
	{
		ReleaseDC(p_hwnd, p_hdc);
		return false;
	}
	if (!SetPixelFormat(p_hdc, pixelFormat, &pfd))
	{
		ReleaseDC(p_hwnd, p_hdc);
		return false;
	}

	p_hglrc = wglCreateContext(p_hdc);
	if (!p_hglrc)
	{
		return false;
	}
	if (!wglMakeCurrent(p_hdc, p_hglrc))
	{
		return false;
	}
	// glew 初始化
	glewInit();
	return true;
}

void Render()
{
	//******以下内容为1.0版本绘制方法，高版本的不建议这样绘制了，尽量使用glDraw*系列函数以提高性能
	//设置点
	//以画线方式绘制
	glBegin(GL_TRIANGLE_STRIP);
	//中心端点，为红色
	glColor3f(1, 0, 0); glVertex2f(-1.0f, -1.0f);
	//到左上角中点端点，为绿色
	glColor3f(0, 1, 0); glVertex2f(0.0f, 1.0f);
	//到上方中点端点，为蓝色
	glColor3f(0, 0, 1); glVertex2f(1.0f, -1.0f);
	glEnd();
}

// 编译顶点着色器与片段着色器(像素着色器)
// 顶点着色器用来绘制图形， 片段着色器用来上色
void CompileShader(unsigned int& shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource) {
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
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	Logger::Get().PrintLog(shaderProgram, GL_LINK_STATUS);

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UpdateWindow(HWND hwnd, HDC hdc, HGLRC hglrc)
{
	RECT rc;
	GetClientRect(hwnd, &rc);

	g_width = rc.right - rc.left;
	g_height = rc.bottom - rc.top;

	wglMakeCurrent(hdc, hglrc);//关联gl的DC到窗口的hdc。如果是单DC，只需关联一次即可
	glViewport(0, 0, g_width, g_height);//设置视口

	glClearColor(background.R, background.G, background.B, background.A);//设置清屏时的颜色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清屏。这里清掉颜色缓冲区与深度缓冲区
}