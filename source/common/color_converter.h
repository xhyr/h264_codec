#pragma once

#include <cstdint>
#include <tuple>

#include "global_defines.h"

__codec_begin

struct ColorConverter
{
	static std::tuple<uint8_t, uint8_t, uint8_t> RGB2YUV709Full(int r, int g, int b);

};

__codec_end