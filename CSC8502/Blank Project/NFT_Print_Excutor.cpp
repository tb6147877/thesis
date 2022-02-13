#include "NFT_Print_Excutor.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"
#include "../nclgl/FileOperator.h"

NFT_Print_Excutor::NFT_Print_Excutor(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :NFT_Excutor(width, height, vertPath, fragPath) {

}
NFT_Print_Excutor::~NFT_Print_Excutor() {
	for (int i = 0; i <m_resLog.size(); i++)
	{
		delete m_resLog[i];
		m_resLog[i] = nullptr;
	}
	m_resLog.empty();
}

void NFT_Print_Excutor::GenerateNFTs() {
	if (m_isCompleted)
	{
		return;
	}
	
	std::string path{ "C:/Users/tb614/Documents/Awei Fashion Man/Awei_Part1/Awei_Part1_Final/" };
	std::vector<std::string> filePath;
	std::vector<std::string> fileName;
	CommonTool::GetPngFiles(FILES_PATH, filePath, fileName);

	for (int i = 0; i < PREFIX_02.size(); i++)
	{
		for (int j = 0; j < GROUP_NUM; j++)
		{
			m_bg = LoadTextureFromFile(filePath[counter].c_str());

			glClear(GL_COLOR_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(m_nft_shader->GetProgram());

			glUniform1i(glGetUniformLocation(m_nft_shader->GetProgram(), "diffTex0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_bg);
			m_quad->Draw();

			std::string str = PREFIX_01;
			str.append(PREFIX_02[i]).append("-").append(std::to_string(j));
			std::string ser = std::to_string(counter);
			std::string name = PREFIX_NAME;
			name.append(str);

			LogData* log = new LogData;
			log->ID = ser;
			log->Serial_Num = str;
			log->Name = name;
			log->FileName = fileName[counter];
			m_resLog.push_back(log);

			m_textRender->RenderText(str, 10, 990, 0.5, Vector3{ 1.0f,1.0f,1.0f });

			std::string temp = path;
			temp.append(log->FileName).append("_").append(str).append(".png");
			SerializeTexture(temp.c_str());
			counter++;

		/*	if (counter==10)
			{
				RecordLog(path);
				m_isCompleted = true;
				return;
			}*/
			
		}
	}

	RecordLog(path);
	m_isCompleted = true;

}

void NFT_Print_Excutor::RecordLog(const std::string& path) {
	std::string temp{ "" };
	for (size_t i = 0; i < m_resLog.size(); i++)
	{
		temp += (m_resLog[i]->ID + ",");
		temp += (m_resLog[i]->Serial_Num + ",");
		temp += (m_resLog[i]->Name + ",");
		temp += (m_resLog[i]->FileName + ",\n");
	}
	FileOperator::writeFile(temp, path + "log.csv");
}