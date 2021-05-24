#pragma once

#include <glad/glad.h>

class DepthPreHelper
{
public:
	DepthPreHelper(const int width, const int height);
	~DepthPreHelper();

	GLuint GetFBO() { return m_depthFBO; }
	GLuint GetDepthTex() { return m_depthTex; }
private:
	int m_width, m_height;
	GLuint m_depthFBO, m_depthTex;
};

