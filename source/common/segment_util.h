#pragma once

#include <cstdint>
#include <utility>

#include "global_defines.h"

__codec_begin

struct SegmentUtil
{
	template<size_t block_width, size_t block_height>
	static std::tuple<uint32_t, uint32_t> GetLumaDataRect(uint8_t segment_index)
	{
		uint32_t width_in_segment = 16 / block_width;
		uint32_t start_x = (segment_index % width_in_segment) * block_width;
		uint32_t start_y = (segment_index / width_in_segment) * block_height;
		return { start_x, start_y };
	}

	template<size_t block_width, size_t block_height>
	static std::tuple<uint32_t, uint32_t> GetLumaDataRect(uint8_t segment_index, uint8_t sub_segment_index)
	{
		uint32_t width_in_sub_segment = 8 / block_width;
		uint32_t start_x =  (segment_index % 2) * 8 + (sub_segment_index % width_in_sub_segment) * block_width;
		uint32_t start_y = (segment_index / 2) * 8 + (sub_segment_index / width_in_sub_segment) * block_height;
		return { start_x, start_y};
	}

	template<size_t block_width, size_t block_height>
	static std::tuple<uint32_t, uint32_t> GetChromaDataRect(uint8_t segment_index)
	{
		uint32_t width_in_segment = 8 / block_width;
		uint32_t start_x = (segment_index % width_in_segment) * block_width;
		uint32_t start_y = (segment_index / width_in_segment) * block_height;
		return { start_x, start_y};
	}
};

__codec_end