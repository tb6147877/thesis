#include "SelectActiveClusterHelper.h"


SelectActiveClusterHelper::SelectActiveClusterHelper() {
	glGenFramebuffers(1, &m_FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


SelectActiveClusterHelper::~SelectActiveClusterHelper() {

}