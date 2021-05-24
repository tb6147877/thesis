#include "DeferredRenderingHelper.h"

DeferredRenderingHelper::DeferredRenderingHelper(const int width, const int height) 
	:m_width{ width }, m_height{height}
{
	glGenFramebuffers(1, &m_gBufferFBO);
	glGenFramebuffers(1, &m_lightingFBO);

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
	
	GenerateScreenTexture(m_gBufferDepthTex, 0);//depth attachment
	GenerateScreenTexture(m_gBufferColorTex, 1);//within 0-1,use rgba
	GenerateScreenTexture(m_gBufferNormalTex, 2);//may beyond 0-1,use rgba16f

	GenerateScreenTexture(m_lightingDiffTex, 2);//may beyond 0-1,use rgba16f
	GenerateScreenTexture(m_lightingSpecTex, 2);//may beyond 0-1,use rgba16f

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


void DeferredRenderingHelper::GenerateScreenTexture(GLuint& tex, const int type) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format, t_type;
	switch (type)
	{
	case 0:
		format = GL_DEPTH_COMPONENT24;
		t_type = GL_DEPTH_COMPONENT;
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, t_type, GL_FLOAT, NULL);
		break;
	case 1:
		format = GL_RGBA;
		t_type = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, t_type, GL_UNSIGNED_BYTE, NULL);
		break;
	case 2:
		format = GL_RGBA16F;
		t_type = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, t_type, GL_FLOAT, NULL);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

}