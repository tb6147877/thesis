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
	const int DIFFERENT_NUMBER_BIAS{ 3 };//这里的数字是指有几个元素不一样才能称之为元素不一样
	GLuint m_bg, m_ele1, m_ele2, m_ele3, m_ele4;
	bool m_isCompleted{ false };
};

