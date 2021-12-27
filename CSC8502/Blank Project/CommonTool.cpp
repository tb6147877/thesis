#include "CommonTool.h"

int CommonTool::GetRamdom(int min, int max) {
	std::uniform_int_distribution<> die{ min, max };
	return die(mersenne);
}

bool CommonTool::IsHitProbability(const int num) {
	int temp = CommonTool::GetRamdom(0, 100);
	if (temp<=num)
	{
		return true;
	}
	return false;
}


std::string CommonTool::GetMd5(const std::string& str) {
	return MD5(str).hexdigest();
}