#pragma once

#include <glad/glad.h>

class SelectActiveClusterHelper
{
public:
	SelectActiveClusterHelper();
	~SelectActiveClusterHelper();

	GLuint GetFBO() { return m_FBO; }

private:
	GLuint m_FBO;

};

