#pragma once

#include <random>
#include <ctime>
#include <string>
#include <unordered_map>
#include <io.h>
#include <iostream>
#include "md5.h"

static std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

class CommonTool
{
public:
	//contain a and b
	static int GetRamdom(int min, int max);

	static bool IsHitProbability(const int num);

	static void GetPngFiles(const std::string& folderPath, std::vector<std::string>& filesWholePath, std::vector<std::string>& filesName);

	template<typename T>
	static size_t CalculateHash(T target) {
		std::size_t result = std::hash<T>{}(target);
		return result;
	}

	static auto get_URBG()
	{
		std::random_device rd;
		// 使用random_device生成seed
		std::mt19937 g(rd());

		return g;
	}

	template <typename T>
	static void ShuffleContainer(std::vector<T>& container)
	{
		std::shuffle(container.begin(), container.end(), get_URBG());
	}

	static std::string GetMd5(const std::string& str);

	static std::vector<std::string> Split(const std::string& str, const std::string& delim);

};

