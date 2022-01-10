#pragma once

#include <glad/glad.h>
#include <string>


#include "TextRender.h"
#include "CommonTool.h"

class Shader;
class Quad;

//一个图片文件包含的所有信息
struct NFT_SourceFile_Data
{
	std::string FolderPath{ "" };
	std::string FilePath{ "" };
	std::string FileName{ "" };//todo:一个元素多种颜色的算法，尝试只通过将这个字段变成一个数组然后轮到这个元素时在这个数组里随机一个元素实现这个算法
};

//一个元素图层包含的所有信息
struct NFT_SourceFile_Cfg {
	std::string FolderPath{ "" };
	std::vector<NFT_SourceFile_Data*> DataArr;
	int Probability{ 100 }; //?%
	void InitDataArr(const std::vector<std::string>& filesPath, const std::vector<std::string>& filesName, const std::string& folderPath);
};

//一张成品NFT里面包含的所有信息
struct NFT_ResultData {
	int ID{ 0 };
	int FeatureNum{ 0 };
	std::string FileCode{ "" };
	std::string Hash{ "" };
	std::vector<std::string> FeatureFlags;
};

class NFT_Excutor
{
public:
	NFT_Excutor(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	virtual ~NFT_Excutor();

	virtual void GenerateNFTs() = 0;
	void InitFilesCfg(const std::vector<std::pair<std::string, int>>& data);

protected:
	int m_width, m_height;
	Shader* m_nft_shader;
	GLuint m_resultFBO, m_resultTex;
	Quad* m_quad;
	std::vector<NFT_SourceFile_Cfg> m_file_cfgs;//有多个元素图层
	int m_total_nft_num{ 0 };
	std::vector<int> m_nft_serial_num;
	std::vector<NFT_ResultData*> m_nft_results;//输出NFT的结果
	TextRender* m_textRender{nullptr};


	void InitShader(const std::string& vertPath, const std::string& fragPath);
	void InitFBO(const int width, const int height);
	void GenerateScreenTexture(GLuint& tex, const int type);
	GLuint LoadTextureFromFile(char const* path);
	void SerializeTexture(char const* path);
	bool IsFileCodeRepeated(const std::string& fileCode);
	void RecordNFTResult(const std::string& folderPath);
	bool IsNFTSame(const int diffNum,const std::vector<std::string>& flags);
};

