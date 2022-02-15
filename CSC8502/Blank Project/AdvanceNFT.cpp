#include "AdvanceNFT.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"
#include "../nclgl/FileOperator.h"
#include <format>


AdvanceNFT::AdvanceNFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :NFT_Excutor(width, height, vertPath, fragPath) {
	m_outputPath = "C:/Users/tb614/Desktop/awei_test_result/";
}
AdvanceNFT::~AdvanceNFT() {

}

void AdvanceNFT::GenerateNFTs() {
	if (m_isCompleted)
	{
		return;
	}

	std::string path{ m_outputPath };
	int counter = 0;

	std::string props{ "" };

	for (size_t i = 0; i < m_file_cfgs[0].DataArr.size(); i++)
	{
		for (size_t j = 0; j < m_file_cfgs[1].DataArr.size(); j++)
		{	
			//1.准备资源
			props = "";
			
			//int num = SelectOneColor(m_file_cfgs[0].DataArr[i]->FilePathArr);
			int num = SelectOneColor(m_file_cfgs[0].DataArr[i]->FileProbability);
			std::string tempPath = m_file_cfgs[0].DataArr[i]->FilePathArr[num];
			m_bg = LoadTextureFromFile(tempPath.c_str());
			props += (std::to_string(m_file_cfgs[0].DataArr[i]->FileID[num]) + "_");
			AddOneElementAppearRecord(m_file_cfgs[0].DataArr[i]->FileID[num]);
			//bg_name = m_file_cfgs[0].DataArr[i]->FileNameArr[num];

			//num = SelectOneColor(m_file_cfgs[1].DataArr[j]->FilePathArr);
			num = SelectOneColor(m_file_cfgs[1].DataArr[j]->FileProbability);
			tempPath = m_file_cfgs[1].DataArr[j]->FilePathArr[num];
			m_ele1 = LoadTextureFromFile(tempPath.c_str());
			props += (std::to_string(m_file_cfgs[1].DataArr[j]->FileID[num]) + "_");
			AddOneElementAppearRecord(m_file_cfgs[1].DataArr[j]->FileID[num]);
			//ele1_name = m_file_cfgs[1].DataArr[j]->FileNameArr[num];


			//2.绘制
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

			//3.序列化
			std::string temp = path;
			temp.append(std::to_string(m_nft_serial_num[counter])).append("_").append(".png");
			SerializeTexture(temp.c_str());
			FileOperator::appendFile(std::to_string(m_nft_serial_num[counter])+","+props + ",\n", path + "NFT Details.csv");

			counter++;
		}
	}

	GenerateElementRealAppearNumberTable();
	m_isCompleted = true;
}