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

	std::string path{ "C:/Users/tb614/Desktop/awei_test_result/" };
	//std::string path{"C:/Users/tb614/Desktop/Awei_First/result_raw/"};
	int counter = 0;

	std::string bg_name{ "" };
	std::string ele1_name{ "" };
	std::string ele2_name{ "" };
	std::string ele3_name{ "" };
	std::string ele4_name{ "" };

	for (int i = 0; i < m_file_cfgs[0].DataArr.size(); i++)
	{
		int num = SelectOneColor(m_file_cfgs[0].DataArr[i]->FilePathArr);
		std::string tempPath = m_file_cfgs[0].DataArr[i]->FilePathArr[num];
		m_bg = LoadTextureFromFile(tempPath.c_str());
		bg_name = m_file_cfgs[0].DataArr[i]->FileNameArr[num];

		for (int j = 0; j < m_file_cfgs[1].DataArr.size(); j++)
		{
			num = SelectOneColor(m_file_cfgs[1].DataArr[j]->FilePathArr);
			tempPath = m_file_cfgs[1].DataArr[j]->FilePathArr[num];
			m_ele1 = LoadTextureFromFile(tempPath.c_str());
			ele1_name = m_file_cfgs[1].DataArr[j]->FileNameArr[num];

			for (int k = 0; k < m_file_cfgs[2].DataArr.size(); k++)
			{
				num = SelectOneColor(m_file_cfgs[2].DataArr[k]->FilePathArr);
				tempPath = m_file_cfgs[2].DataArr[k]->FilePathArr[num];
				m_ele2 = LoadTextureFromFile(tempPath.c_str());
				ele2_name = m_file_cfgs[2].DataArr[k]->FileNameArr[num];

				for (int m = 0; m < m_file_cfgs[3].DataArr.size(); m++)
				{
					num = SelectOneColor(m_file_cfgs[3].DataArr[m]->FilePathArr);
					tempPath = m_file_cfgs[3].DataArr[m]->FilePathArr[num];
					m_ele3 = LoadTextureFromFile(tempPath.c_str());
					ele3_name = m_file_cfgs[3].DataArr[m]->FileNameArr[num];

					for (int n = 0; n < m_file_cfgs[4].DataArr.size(); n++)
					{
						num = SelectOneColor(m_file_cfgs[4].DataArr[n]->FilePathArr);
						tempPath = m_file_cfgs[4].DataArr[n]->FilePathArr[num];
						m_ele4 = LoadTextureFromFile(tempPath.c_str());
						ele4_name = m_file_cfgs[4].DataArr[n]->FileNameArr[num];


						//===============================================
						int featureNum{ 0 };
						NFT_ResultData* result = new NFT_ResultData;
						std::string fileCode{ "" };

						bool is_bg_draw{ false };
						bool is_ele1_draw{ false };
						bool is_ele2_draw{ false };
						bool is_ele3_draw{ false };
						bool is_ele4_draw{ false };


						glClear(GL_COLOR_BUFFER_BIT);

						glDisable(GL_DEPTH_TEST);
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						glUseProgram(m_nft_shader->GetProgram());

						if (CommonTool::IsHitProbability(m_file_cfgs[0].Probability))
						{
							//result->FeatureFlags.push_back(m_file_cfgs[0].DataArr[i]->FileName);
							//result->FeatureFlags.push_back(bg_name);
							featureNum++;
							fileCode.append(bg_name);
							is_bg_draw = true;
						}
						else {
							//result->FeatureFlags.push_back("");
						}

						if (CommonTool::IsHitProbability(m_file_cfgs[1].Probability))
						{
							//result->FeatureFlags.push_back(m_file_cfgs[1].DataArr[j]->FileName);
							//result->FeatureFlags.push_back(ele1_name);
							featureNum++;
							fileCode.append(ele1_name);
							is_ele1_draw = true;
						}
						else {
							//result->FeatureFlags.push_back("");
						}

						if (CommonTool::IsHitProbability(m_file_cfgs[2].Probability))
						{
							//result->FeatureFlags.push_back(ele2_name);
							featureNum++;
							fileCode.append(ele2_name);
							is_ele2_draw = true;
						}
						else {
							//result->FeatureFlags.push_back("");
						}

						if (CommonTool::IsHitProbability(m_file_cfgs[3].Probability))
						{
							//result->FeatureFlags.push_back(ele3_name);
							featureNum++;
							fileCode.append(ele3_name);
							is_ele3_draw = true;
						}
						else {
							//result->FeatureFlags.push_back("");
						}

						if (CommonTool::IsHitProbability(m_file_cfgs[4].Probability))
						{
							//result->FeatureFlags.push_back(ele4_name);
							featureNum++;
							fileCode.append(ele4_name);
							is_ele4_draw = true;
						}
						else {
							//result->FeatureFlags.push_back("");
						}


						//����ж���Ϊ�˷�ֹ��Ԫ���Ǽ�����Ⱦ������£�������NFT��������ȫ������Ϸ�ʽʱ����һ��������ʳ��ֵ�forѭ�����ж��NFT��Ԫ��û������϶����NFT�ظ�����һ�жϽ�����������
						/*if (IsFileCodeRepeated(fileCode))
						{
							continue;
						}*/

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
						if (is_ele3_draw)
						{
							glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, m_ele3);
							m_quad->Draw();
						}
						if (is_ele4_draw)
						{
							glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, m_ele4);
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
		}
	}

	RecordNFTResult(path);

	m_isCompleted = true;
}