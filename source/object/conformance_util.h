#pragma once

#include <cstdint>
#include <utility>

#include "global_defines.h"

__codec_begin

enum LevelId
{
	LEVEL_1 = 0,
	LEVEL_1b = 1,
	LEVEL_1_1 = 2,
	LEVEL_1_2 = 3,
	LEVEL_1_3 = 4,
	LEVEL_2 = 5,
	LEVEL_2_1 = 6,
	LEVEL_2_2 = 7,
	LEVEL_3 = 8,
	LEVEL_3_1 = 9,
	LEVEL_3_2 = 10,
	LEVEL_4 = 11,
	LEVEL_4_1 = 12,
	LEVEL_4_2 = 13,
	LEVEL_5 = 14,
	LEVEL_5_1 = 15,
	LEVEL_5_2 = 16,
	LEVEL_6 = 17,
	LEVEL_6_1 = 18,
	LEVEL_6_2 = 19,
	LEVEL_UNCONSTRAINED = 20
};

struct ConformanceUtil
{
	static int GetLevelIndex(int profile_idc, int level_idc);

	static std::pair<int, int> GetMVVerticalLimit(int profile_idc, int level_idc);

	static std::pair<int, int> GetMVHorizontalLimit(int profile_idc, int level_idc);
};

__codec_end