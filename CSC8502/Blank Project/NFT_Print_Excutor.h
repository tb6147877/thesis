#pragma once

#include "NFT_Excutor.h"
#include <vector>
#include <string>

/*
awei系列的成品图上面加编号的代码
*/

struct LogData
{
	std::string ID{""};
	std::string Serial_Num{ "" };
	std::string Name{""};
	std::string FileName{ "" };
};

class NFT_Print_Excutor : public NFT_Excutor
{
public:
	NFT_Print_Excutor(const int width, const int height, const std::string& vertPath, const std::string& fragPath);
	~NFT_Print_Excutor();

	void GenerateNFTs() override;

private:
	const std::string PREFIX_01{ "P1-" };
	const std::vector<std::string> PREFIX_02{ "A","B","C","D","E","F","G","H","I","J" };
	const std::string PREFIX_NAME{ "Awei Fashion Man Part1 #" };
	const int GROUP_NUM{ 100 };
	int counter{ 0 };
	GLuint m_bg;
	bool m_isCompleted{ false };
	std::vector<LogData*> m_resLog;
	std::string FILES_PATH{"C:/Users/tb614/Documents/Awei Fashion Man/Awei_Part1/Awei_Part1_Raw"};


	void RecordLog(const std::string& path);
};

