#pragma once
#include "NFT_Excutor.h"

#include <vector>
#include <string>


//awei系列的一个测试代码，现在也没用了，废弃

class Awei_NFT : public NFT_Excutor
{
public:
	Awei_NFT(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	~Awei_NFT();

	void GenerateNFTs() override;

private:
	//TODO: 这里编程程序根据文件命名规范自动查找
	std::vector<std::string> m_bg_files{
		"C:/Users/tb614/Desktop/Awei/Awei/bg_blue.png",
		"C:/Users/tb614/Desktop/Awei/Awei/bg_vpink.png"
	};

	std::vector<std::string> m_ele1_files{
		"C:/Users/tb614/Desktop/Awei/Awei/ele1_green.png",
		"C:/Users/tb614/Desktop/Awei/Awei/ele1_ygreen.png",
	};

	std::vector<std::string> m_ele2_files{
		"C:/Users/tb614/Desktop/Awei/Awei/ele2_b.png",
		"C:/Users/tb614/Desktop/Awei/Awei/ele2_band.png",
	};

	std::vector<std::string> m_ele3_files{
		"C:/Users/tb614/Desktop/Awei/Awei/ele3_down.png",
		"C:/Users/tb614/Desktop/Awei/Awei/ele3_glass.png",
	};

	std::vector<std::string> m_ele4_files{
		"C:/Users/tb614/Desktop/Awei/Awei/ele4_fire.png",
		"C:/Users/tb614/Desktop/Awei/Awei/ele4_g.png",
	};



	GLuint m_bg, m_ele1, m_ele2, m_ele3, m_ele4;
	bool m_isCompleted{ false };
};

