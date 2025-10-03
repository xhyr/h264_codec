#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct InterpolateUtil
{
	//x, y is in 1/8 pixel
	static std::vector<uint8_t> InterpolateChromaBlock(std::shared_ptr<uint8_t[]> source_data, int start_x, int start_y, int block_width, int block_height, int width, int height);

private:
	static uint8_t GetInterploatedChromaPixel(std::shared_ptr<uint8_t[]> source_data, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

};

__codec_end