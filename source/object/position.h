#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct Position
{
	constexpr Position(int _x = 0, int _y = 0) : x(_x), y(_y)
	{ }

	int x;
	int y;
};

__codec_end