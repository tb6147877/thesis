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
	//TODO: 这里编程程序根据文件命名规范自动查找
	std::vector<std::string> m_bg_files{
		"C:/Users/tb614/Desktop/resourses/resourses/bg_blue.png",
		"C:/Users/tb614/Desktop/resourses/resourses/bg_red.png",
		"C:/Users/tb614/Desktop/resourses/resourses/bg_yellow.png",
	};

	std::vector<std::string> m_ele1_files{
		"C:/Users/tb614/Desktop/resourses/resourses/ele_1_leaf.png",
		"C:/Users/tb614/Desktop/resourses/resourses/ele_1_pink_circle.png",
	};

	std::vector<std::string> m_ele2_files{
		"C:/Users/tb614/Desktop/resourses/resourses/ele_2_rainbow.png",
		"C:/Users/tb614/Desktop/resourses/resourses/ele_2_smile.png",
	};

	GLuint m_bg, m_ele1, m_ele2;
	bool m_isCompleted{ false };
};

