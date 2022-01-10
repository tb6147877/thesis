#include "TestNFT.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"

#include <format>

TestNFT::TestNFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :NFT_Excutor(width, height, vertPath,fragPath){
}


TestNFT::~TestNFT() {

}


void TestNFT::GenerateNFTs() {
	if (m_isCompleted)
	{
		return;
	}

	std::string path{"C:/Users/tb614/Desktop/tests1/"};
	int counter = 0;

	for (int i = 0; i < m_file_cfgs[0].DataArr.size(); i++)
	{
		m_bg = LoadTextureFromFile(m_file_cfgs[0].DataArr[i]->FilePath.c_str());

		for (int j = 0; j < m_file_cfgs[1].DataArr.size(); j++)
		{
			m_ele1 = LoadTextureFromFile(m_file_cfgs[1].DataArr[j]->FilePath.c_str());

			for (int k = 0; k < m_file_cfgs[2].DataArr.size(); k++)
			{
				m_ele2 = LoadTextureFromFile(m_file_cfgs[2].DataArr[k]->FilePath.c_str());

				int featureNum{ 0 };
				NFT_ResultData* result=new NFT_ResultData;
				std::string fileCode{ "" };

				bool is_bg_draw{ false };
				bool is_ele1_draw{ false };
				bool is_ele2_draw{ false };


				glClear(GL_COLOR_BUFFER_BIT);

				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glUseProgram(m_nft_shader->GetProgram());

				if (CommonTool::IsHitProbability(m_file_cfgs[0].Probability))
				{
					result->FeatureFlags.push_back(m_file_cfgs[0].DataArr[i]->FileName);
					featureNum++;
					fileCode.append(m_file_cfgs[0].DataArr[i]->FileName);
					is_bg_draw = true;
				}
				else {
					result->FeatureFlags.push_back("");
				}

				if (CommonTool::IsHitProbability(m_file_cfgs[1].Probability))
				{
					result->FeatureFlags.push_back(m_file_cfgs[1].DataArr[j]->FileName);
					featureNum++;
					fileCode.append(m_file_cfgs[1].DataArr[j]->FileName);
					is_ele1_draw = true;
				}
				else {
					result->FeatureFlags.push_back("");
				}
				
				if (CommonTool::IsHitProbability(m_file_cfgs[2].Probability))
				{
					result->FeatureFlags.push_back(m_file_cfgs[2].DataArr[k]->FileName);
					featureNum++;
					fileCode.append(m_file_cfgs[2].DataArr[k]->FileName);
					is_ele2_draw = true;
				}
				else {
					result->FeatureFlags.push_back("");
				}


				//����ж���Ϊ�˷�ֹ��Ԫ���Ǽ�����Ⱦ������£�������NFT��������ȫ������Ϸ�ʽʱ����һ��������ʳ��ֵ�forѭ�����ж��NFT��Ԫ��û������϶����NFT�ظ�����һ�жϽ�����������
				if (IsFileCodeRepeated(fileCode))
				{
					continue;
				}

				//����ж���Ϊ��˵������Ԫ�ز�ͬ���ǲ���ΪNFT��ͬ��������NFT��������ȫ������Ϸ�ʽʱ��һ��Ԫ�صĲ�ͬ���Ǿ���Ϊ���NFT������NFT��ͬ��
				//��������Ҳ����ͨ������ж�ָ����������Ԫ�ز�ͬ��NFT�Ų�ͬ
				/*if (IsNFTSame(DIFFERENT_NUMBER_BIAS, result->FeatureFlags))
				{
					continue;
				}*/

				//��ȫ�����������ж�����draw
				if (is_bg_draw)
				{
					glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_bg);
					m_quad->Draw();
				}
				if (is_ele1_draw)
				{
					glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_ele1);
					m_quad->Draw();
				}
				if (is_ele2_draw)
				{
					glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_ele2);
					m_quad->Draw();
				}

				result->FeatureNum = featureNum;
				result->ID = m_nft_serial_num[counter];
				result->FileCode = fileCode;
				result->Hash = CommonTool::GetMd5(fileCode);
				m_nft_results.push_back(result);

				//m_textRender->RenderText(result->Hash, 200, 200, 0.2, Vector3{ 1.0f,1.0f,1.0f });

				std::string temp = path;
				temp.append(std::to_string(m_nft_serial_num[counter])).append("_").append(std::to_string(featureNum)).append("_").append(result->Hash).append(".png");


				SerializeTexture(temp.c_str());
				counter++;
			}
		}
	}

	RecordNFTResult(path);

	m_isCompleted = true;
}