#pragma once

#include "NFT_Excutor.h"
#include <vector>
#include <string>


class AdvanceNFT :public NFT_Excutor
{
public:
	AdvanceNFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	~AdvanceNFT();

	void GenerateNFTs() override;

private:
	GLuint m_bg, m_ele1, m_ele2, m_ele3, m_ele4;
	bool m_isCompleted{ false };
};

