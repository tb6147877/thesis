#include "Awei_NFT.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"

#include <format>

Awei_NFT::Awei_NFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :NFT_Excutor(width, height, vertPath, fragPath) {
}


Awei_NFT::~Awei_NFT() {

}

void Awei_NFT::GenerateNFTs() {

	if (m_isCompleted)
	{
		return;
	}

	std::string path{ "C:/Users/tb614/Desktop/awei_test/" };
	int counter = 0;

	for (int i = 0; i < m_bg_files.size(); i++)
	{
		m_bg = LoadTextureFromFile(m_bg_files[i].c_str());
		for (int j = 0; j < m_ele1_files.size(); j++)
		{
			m_ele1 = LoadTextureFromFile(m_ele1_files[j].c_str());
			for (int k = 0; k < m_ele2_files.size(); k++)
			{
				m_ele2 = LoadTextureFromFile(m_ele2_files[k].c_str());

				for (int m = 0; m < m_ele3_files.size(); m++)
				{
					m_ele3= LoadTextureFromFile(m_ele3_files[m].c_str());
					for (int n = 0; n < m_ele4_files.size(); n++)
					{
						m_ele4= LoadTextureFromFile(m_ele4_files[n].c_str());

						glClear(GL_COLOR_BUFFER_BIT);

						glDisable(GL_DEPTH_TEST);
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						glUseProgram(m_nft_shader->GetProgram());


						glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, m_bg);
						m_quad->Draw();

						glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, m_ele1);
						m_quad->Draw();

						glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, m_ele2);
						m_quad->Draw();

						glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, m_ele3);
						m_quad->Draw();

						glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, m_ele4);
						m_quad->Draw();

						std::string temp = path;
						temp.append(std::to_string(counter)).append("_r.png");

						SerializeTexture(temp.c_str());
						counter++;
					}

				}

				
			}
		}
	}

	m_isCompleted = true;
}