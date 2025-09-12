#pragma once

#include <cstdint>
#include <vector>

#include "codec_types.h"

__codec_begin

struct BlockUtil
{
	static uint32_t CalculateBlockIndex(uint32_t block_8x8, uint32_t block_4x4);

	static std::vector<uint32_t> GetBlockIndices(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, uint32_t frame_width_in_block);

	static bool GetBlockNeighborIndex(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t frame_width_in_block, BlockNeighborType block_neighbor_type, uint32_t& block_index);

	static uint32_t GetBlockIndex(uint32_t x_in_block, uint32_t y_in_block, uint32_t frame_width_in_block);

	static uint32_t IsPreDecoded(uint32_t new_x_in_block, uint32_t new_y_in_block, uint32_t x_in_block, uint32_t y_in_block);

};

__codec_end