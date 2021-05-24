#pragma once

#include <glad/glad.h>

class DeferredRenderingHelper
{
public:
	DeferredRenderingHelper(const int width, const int height);
	~DeferredRenderingHelper();

	GLuint GetGBufferFBO() { return m_gBufferFBO; }
	GLuint GetLightingFBO() { return m_lightingFBO; }

	GLuint GetGBufferDepthTex() { return m_gBufferDepthTex; }
	GLuint GetGBufferNormalTex() { return m_gBufferNormalTex; }
	GLuint GetGBufferColorTex() { return m_gBufferColorTex; }

	GLuint GetLightingDiffTex() { return m_lightingDiffTex; }
	GLuint GetLightingSpecTex() { return m_lightingSpecTex; }
private:
	GLuint m_gBufferFBO, m_gBufferColorTex, m_gBufferDepthTex, m_gBufferNormalTex;
	GLuint m_lightingFBO, m_lightingDiffTex, m_lightingSpecTex;
	int m_width, m_height;

	//0.depth  1.RGBA   2.RGBA16F
	void GenerateScreenTexture(GLuint& tex, const int type);

};

