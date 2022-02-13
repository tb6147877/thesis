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

void CommonTool::GetPngFiles(const std::string& path, std::vector<std::string>& files, std::vector<std::string>& filesName) {
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)//if has folder,enter
	{
		do
		{
			//there is a folder in the folder
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				//folder name is not "."&&".."
					//. is current directory
					//..is the father directory of current directory
				 //must avoid these cases
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetPngFiles(p.assign(path).append("/").append(fileinfo.name), files, filesName);
			}
			else//if not folder  
			{
				std::string temp{ fileinfo.name };
				std::string tail = ".png";
				if (temp.compare(temp.size() - tail.size(), tail.size(), tail) == 0) {
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
					filesName.push_back(fileinfo.name);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


std::vector<std::string> CommonTool::Split(const std::string& str, const std::string& delim) {
	std::vector<std::string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char* strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char* p = strtok(strs, d);
	while (p) {
		std::string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}

	return res;
}