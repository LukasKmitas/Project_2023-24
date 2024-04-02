#pragma once
#include <vector>
#include <array>
#include <chrono>

struct Global
{
public:

	static const int S_WIDTH{ 1920U };
	static const int S_HEIGHT{ 1080U };
	static int playerCurrency;
	static int enemyCurrency;
};

using vector_1d = std::vector<float>;
using vector_2d = std::vector<std::vector<float>>;
using vector_3d = std::vector<std::vector<std::vector<float>>>;

const int  SCREEN_WITDH = 1920;
const int  SCREEN_HEIGHT = 1080;