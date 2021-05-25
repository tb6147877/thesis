#pragma once

#include <glad/glad.h>

class FinalOutputHelper
{
public:
	FinalOutputHelper(const int width, const int height);
	~FinalOutputHelper();

	GLuint GetFBO() { return m_FBO; }
	GLuint GetTex() { return m_Tex; }
private:
	int m_width, m_height;
	GLuint m_FBO,m_Tex, m_RBO;
};

