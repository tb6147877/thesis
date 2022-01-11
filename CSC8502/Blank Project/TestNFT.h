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
	const int DIFFERENT_NUMBER_BIAS{ 3 };//�����������ָ�м���Ԫ�ز�һ�����ܳ�֮ΪԪ�ز�һ��
	GLuint m_bg, m_ele1, m_ele2, m_ele3, m_ele4;
	bool m_isCompleted{ false };
};

