#pragma once
#include "NFT_Excutor.h"
#include <vector>
#include <string>

class TestNFT : public NFT_Excutor
{
public:
	TestNFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	~TestNFT();

	void GenerateNFTs() override;

private:
	GLuint m_bg, m_ele1, m_ele2;
	bool m_isCompleted{ false };
};

