#include "TestNFT.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"

TestNFT::TestNFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :NFT_Excutor(width, height, vertPath,fragPath){
	m_bg = LoadTextureFromFile(m_bg_files[0].c_str());
	m_ele1 = LoadTextureFromFile(m_ele1_files[0].c_str());
	m_ele2 = LoadTextureFromFile(m_ele2_files[0].c_str());
}


TestNFT::~TestNFT() {

}


void TestNFT::GenerateNFTs() {
	/*if (m_isCompleted)
	{
		return;
	}

	for (int i = 0; i < m_bg_files.size(); i++)
	{
		for (int j = 0; j < m_ele1_files.size(); j++)
		{
			for (size_t i = 0; i < m_ele2_files.size(); i++)
			{
				glClear(GL_COLOR_BUFFER_BIT);

			}
		}
	}

	m_isCompleted = true;*/

	glClear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_nft_shader->GetProgram());

	
	glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bg);
	m_quad->Draw();

	/*glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
	glActiveTexture(GL_TEXTURE0);*/
	glBindTexture(GL_TEXTURE_2D, m_ele1);
	m_quad->Draw();

	/*glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
	glActiveTexture(GL_TEXTURE0);*/
	glBindTexture(GL_TEXTURE_2D, m_ele2);
	m_quad->Draw();
}