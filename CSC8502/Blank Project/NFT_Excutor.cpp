#include "NFT_Excutor.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Quad.h"
#include "../nclgl/FileOperator.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include <algorithm>
#include <map>

int NFT_SourceFile_Cfg::count = 0;

void NFT_SourceFile_Cfg::InitDataArr(const std::vector<std::string>& filesPath, const std::vector<std::string>& filesName, const std::string& folderPath) {
	std::map<char, NFT_SourceFile_Data*> map;

	for (int i = 0; i < filesPath.size(); i++)
	{
		std::vector<std::string> strs = CommonTool::Split(filesName[i], "_");
		int r1= atoi(strs[1].c_str());
		int r2 = atoi(strs[2].c_str());

		char c = filesName[i][0];
		auto iter = std::find_if(map.begin(), map.end(), [&](const std::pair<char, NFT_SourceFile_Data*> item)->bool {
			return (item.first==c);
			});
		if (iter!=map.end())
		{
			map[c]->FileNameArr.push_back(filesName[i]);
			map[c]->FilePathArr.push_back(filesPath[i]);
			map[c]->FileProbability.push_back(std::pair<int, int>{r1, r2});
			map[c]->FileID.push_back(count);
		}
		else {
			NFT_SourceFile_Data* ptr = new NFT_SourceFile_Data();
			ptr->FolderPath = folderPath;
			ptr->FileNameArr.push_back(filesName[i]);
			ptr->FilePathArr.push_back(filesPath[i]);
			ptr->FileProbability.push_back(std::pair<int, int>{r1, r2});
			ptr->FileID.push_back(count);
			map.insert(std::pair<char, NFT_SourceFile_Data*>(c, ptr));
		}
		count++;
	}

	for (const auto& item : map)
	{
		DataArr.push_back(item.second);
	}
}


NFT_Excutor::NFT_Excutor(const int width, const int height, const std::string& vertPath, const std::string& fragPath) :m_width{ width }, m_height{ height } {
	InitFBO(width, height);
	InitShader(vertPath, fragPath);
	m_quad = new Quad();
	m_textRender = new TextRender(width, height);
	if (m_pic_data!=nullptr)
	{
		delete[] m_pic_data;
		m_pic_data = nullptr;
	}
}

NFT_Excutor::~NFT_Excutor() {
	if (m_nft_shader != nullptr)
	{
		delete m_nft_shader;
		m_nft_shader = nullptr;
	}

	if (m_quad != nullptr)
	{
		delete m_quad;
		m_quad = nullptr;
	}

	for (int i = 0; i < m_file_cfgs.size(); i++)
	{
		for (int j = 0; j < m_file_cfgs[i].DataArr.size(); j++)
		{
			delete m_file_cfgs[i].DataArr[j];
			m_file_cfgs[i].DataArr[j] = nullptr;
		}
	}

	if (m_textRender!=nullptr)
	{
		delete m_textRender;
		m_textRender = nullptr;
	}
}

void NFT_Excutor::InitShader(const std::string& vertPath, const std::string& fragPath) {
	m_nft_shader = new Shader(vertPath, fragPath);
}

void NFT_Excutor::InitFBO(const int width, const int height) {
	glGenFramebuffers(1, &m_resultFBO);

	GenerateScreenTexture(m_resultTex, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, m_resultFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resultTex, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NFT_Excutor::GenerateScreenTexture(GLuint& tex, const int type) {
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

GLuint NFT_Excutor::LoadTextureFromFile(char const* path) {
	std::string str{ path };
	auto iter = std::find_if(m_imgInRAM.begin(), m_imgInRAM.end(), [&](const std::pair<std::string,GLuint> item)->bool {
		return (item.first == str);
		});

	if (iter != m_imgInRAM.end())
	{
		std::cout << "Already has this img:" << str << "\n";
		return m_imgInRAM[str];
	}


	stbi_set_flip_vertically_on_load(true);
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	m_imgInRAM.insert(std::pair<std::string, GLuint>{str, textureID});
	return textureID;
}

void NFT_Excutor::SerializeTexture(char const* path) {
	stbi_flip_vertically_on_write(true);
	if (m_pic_data==nullptr)
	{
		m_pic_data = new GLubyte[(3 * m_width * m_height)];
	}
	memset(m_pic_data, 0, 3 * m_width * m_height);

	//glBindFramebuffer(GL_FRAMEBUFFER, m_resultFBO);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_pic_data);

	// Write the PNG image
	int numOfComponents = 3; // RGB
	int strideInBytes = m_width * 3;
	stbi_write_png(path, m_width, m_height, 3, m_pic_data, strideInBytes);
	
}

void NFT_Excutor::InitFilesCfg(const std::vector<std::pair<std::string, int>>& data) {
	m_total_nft_num = 1;
	for (int i = 0; i < data.size(); i++)
	{
		NFT_SourceFile_Cfg cfg;
		cfg.Probability = data[i].second;
		cfg.FolderPath = data[i].first;

		std::vector<std::string> filePath;
		std::vector<std::string> fileName;
		CommonTool::GetPngFiles(cfg.FolderPath, filePath, fileName);
		cfg.InitDataArr(filePath, fileName, cfg.FolderPath);

		m_file_cfgs.push_back(cfg);

		//m_total_nft_num *= filePath.size();
	}

	for (int i = 0; i < m_file_cfgs.size(); i++)
	{
		m_total_nft_num *= m_file_cfgs[i].DataArr.size();
	}

	for (int i = 0; i < m_total_nft_num; i++)
	{
		m_nft_serial_num.push_back(i);
	}

	CommonTool::ShuffleContainer(m_nft_serial_num);
	GenerateImgIDNameTable();
	std::cout << "NFT NUM:" << m_total_nft_num << "\n";
}


void NFT_Excutor::GenerateImgIDNameTable() {
	std::string temp{ "" };
	
	for (size_t i = 0; i < m_file_cfgs.size(); i++)
	{
		for (size_t j = 0; j <m_file_cfgs[i].DataArr.size(); j++)
		{
			for (size_t k = 0; k < m_file_cfgs[i].DataArr[j]->FileNameArr.size(); k++)
			{
				temp += (std::to_string(m_file_cfgs[i].DataArr[j]->FileID[k] )+ ",");
				temp += (m_file_cfgs[i].DataArr[j]->FileNameArr[k] + ",\n");
			}
		}
	}
	FileOperator::writeFile(temp, m_outputPath + "Picture_ID_Name.csv");
}

bool NFT_Excutor::IsFileCodeRepeated(const std::string& fileCode) {
	for (int i = 0; i < m_nft_results.size(); i++)
	{
		if (m_nft_results[i]->FileCode==fileCode)
		{
			return true;
		}
	}

	return false;
}

bool NFT_Excutor::IsNFTSame(const int diffNum, const std::vector<std::string>& flags) {
	/*int result{ 0 };
	for (int i = 0; i < m_nft_results.size(); i++)
	{
		result = 0;
		for (int j = 0; j < m_nft_results[i]->FeatureFlags.size(); j++)
		{
			if (flags[j] == m_nft_results[i]->FeatureFlags[j])
			{
				result++;
			}
		}

		if (result >= diffNum)
		{
			return true;
		}
	}*/

	
	return false;
}

bool SortFunc(NFT_ResultData* a, NFT_ResultData* b) {
	return (a->ID < b->ID);
}

void NFT_Excutor::RecordNFTResult(const std::string& folderPath) {
	std::string temp{ "" };
	std::sort(m_nft_results.begin(), m_nft_results.end(),SortFunc);
	for (size_t i = 0; i < m_nft_results.size(); i++)
	{
		temp += (std::to_string(m_nft_results[i]->ID) + ",");
		temp += (std::to_string(m_nft_results[i]->FeatureNum) + ",");
		//temp += (m_nft_results[i]->FileCode + ",");
		temp += (m_nft_results[i]->Hash + ",\n");
	}
	FileOperator::writeFile(temp, folderPath + "log.csv");
}

void NFT_Excutor::RecordOneNFTResult(const std::string& folderPath) {

}

int NFT_Excutor::SelectOneColor(const std::vector<std::string>& arr) {
	if (arr.size()==0)
	{
		std::cout << "ÔªËØÍ¼Æ¬ÊýÁ¿´íÎó\n";
		return -1;
	}
	int min = 0;
	int max = arr.size() - 1;
	int num = CommonTool::GetRamdom(min, max);
	return num;
}

int NFT_Excutor::SelectOneColor(const std::vector<std::pair<int, int>>& arr) {
	if (arr.size() == 0)
	{
		std::cout << "ÔªËØÍ¼Æ¬ÊýÁ¿´íÎó\n";
		return -1;
	}
	int num = CommonTool::GetRamdom(0, 100);
	for (size_t i = 0; i < arr.size(); i++)
	{
		if (num>=arr[i].first&&num<=arr[i].second)
		{
			return i;
		}
	}

	std::cout << "ÔªËØÍ¼Æ¬¸ÅÂÊÉèÖÃ´íÎó\n";
	return -1;
}