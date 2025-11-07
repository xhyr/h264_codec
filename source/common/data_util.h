#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "block_data.h"

__codec_begin

struct DataUtil
{
	static std::vector<uint8_t> ObtainDataInBlock(std::shared_ptr<uint8_t[]> source_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width);
	
	static std::vector<uint8_t> ObtainDataInBlock(std::shared_ptr<uint8_t[]> source_data, int x, int y, int block_width, int block_height, int width, int height);

	static void SetData(std::shared_ptr<uint8_t[]> total_data, std::shared_ptr<uint8_t[]> block_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width);

	template<typename Ty>
	static void SetData(BlockData<4, 4, Ty>& target_data, const BlockData<2, 2, Ty>& source_data, uint8_t index)
	{
		uint32_t start_x = index % 2 * 2;
		uint32_t start_y = index / 2 * 2;
		for (uint32_t y = start_y; y < start_y + 2; ++y)
			for (uint32_t x = start_x; x < start_x + 2; ++x)
				target_data.SetElement(x, y, source_data.GetElement(x - start_x, y - start_y));

	}

	template<typename Ty>
	static void SetData(BlockData<8, 8, Ty>& target_data, const BlockData<2, 2, Ty>& source_data, uint8_t segment_index, uint8_t sub_segment_index)
	{
		uint32_t start_x = segment_index % 2 * 4 + sub_segment_index % 2 * 2;
		uint32_t start_y = segment_index / 2 * 4 + sub_segment_index / 2 * 2;
		for (uint32_t y = start_y; y < start_y + 2; ++y)
			for (uint32_t x = start_x; x < start_x + 2; ++x)
				target_data.SetElement(x, y, source_data.GetElement(x - start_x, y - start_y));

	}

};

__codec_end