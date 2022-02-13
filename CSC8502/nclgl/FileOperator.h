#pragma once

#include <fstream>
#include <iostream>
#include <string>

class FileOperator
{
public:
	//write content to a specific path
	static void writeFile(const std::string& content, const std::string& path);
	//append content to a specific path
	static void appendFile(const std::string& content, const std::string& path);
private:

};

