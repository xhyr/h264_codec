#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct DataUtil
{
	static std::vector<uint8_t> ObtainDataInBlock(std::shared_ptr<uint8_t[]> source_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width);
	
	static std::vector<uint8_t> ObtainDataInBlock(std::shared_ptr<uint8_t[]> source_data, int x, int y, int block_width, int block_height, int width, int height);

	static void SetData(std::shared_ptr<uint8_t[]> total_data, std::shared_ptr<uint8_t[]> block_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width);

};

__codec_end