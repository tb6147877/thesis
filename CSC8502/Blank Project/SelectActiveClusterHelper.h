#pragma once

#include <glad/glad.h>

class SelectActiveClusterHelper
{
public:
	SelectActiveClusterHelper(const int width, const int height);
	~SelectActiveClusterHelper();

	GLuint GetFBO() { return m_FBO; }
	GLuint GetTex() { return m_clusterIndexTex;}

private:
	GLuint m_FBO, m_clusterIndexTex;

};

