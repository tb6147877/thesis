#pragma once

#include <glad/glad.h>
#include <string>

#include "CommonTool.h"

class Shader;
class Quad;

struct NFT_SourceFile_Data
{
	std::string Prefix{ "" };
	std::string FilePath{ "" };
	std::string FileName{ "" };
};

struct NFT_SourceFile_Cfg {
	std::string Prefix{ "" };
	std::vector<NFT_SourceFile_Data*> DataArr;

	void InitDataArr(const std::vector<std::string>& filesPath, const std::vector<std::string>& filesName);
};

class NFT_Excutor
{
public:
	NFT_Excutor(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	virtual ~NFT_Excutor();

	virtual void GenerateNFTs() = 0;

protected:
	int m_width, m_height;
	Shader* m_nft_shader;
	GLuint m_resultFBO, m_resultTex;
	Quad* m_quad;

	void InitShader(const std::string& vertPath, const std::string& fragPath);
	void InitFBO(const int width, const int height);
	void GenerateScreenTexture(GLuint& tex, const int type);
	GLuint LoadTextureFromFile(char const* path);
	void SerializeTexture(char const* path);
};

