#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

struct ColorData
{
	bool valid;
	uint8_t channel_num;
	uint32_t width;
	uint32_t height;
	std::vector<uint8_t> data;
};

__codec_end