#include "FileOperator.h"


void FileOperator::writeFile(const std::string& content, const std::string& path) {
	std::fstream file;
	file.open(path, std::ios::out);
	file << content;
	file.close();
}