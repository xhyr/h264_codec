#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

struct ColorData
{
	ColorData();
	ColorData(int _width, int _height, int _channel_num, uint8_t* _data);

	bool valid;
	uint8_t channel_num;
	uint32_t width;
	uint32_t height;
	std::vector<uint8_t> data;
};

__codec_end