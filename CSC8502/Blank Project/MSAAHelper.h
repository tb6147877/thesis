#pragma once

#include <glad/glad.h>

class MSAAHelper
{
public:
	MSAAHelper(const int width, const int height);
	~MSAAHelper();

	GLuint GetFBO() { return m_FBO; }

private:
	GLuint m_FBO, m_colorTex,m_depthTex;
};

