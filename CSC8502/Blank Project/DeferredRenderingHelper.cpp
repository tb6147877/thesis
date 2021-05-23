#include "DeferredRenderingHelper.h"

DeferredRenderingHelper::DeferredRenderingHelper(const int width, const int height) 
	:m_width{ width }, m_height{height}
{
	glGenFramebuffers(1, &m_gBufferFBO);
	glGenFramebuffers(1, &m_lightingFBO);

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
	
	GenerateScreenTexture(m_gBufferDepthTex, true);
	GenerateScreenTexture(m_gBufferColorTex);
	GenerateScreenTexture(m_gBufferNormalTex);

	GenerateScreenTexture(m_lightingDiffTex);
	GenerateScreenTexture(m_lightingSpecTex);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightingDiffTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_lightingSpecTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DeferredRenderingHelper::~DeferredRenderingHelper() {}


void DeferredRenderingHelper::GenerateScreenTexture(GLuint& tex, const bool depth) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA16F;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

}