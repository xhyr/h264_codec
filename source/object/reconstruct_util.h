#pragma once

#include <cstdint>

#include "block_data.h"

__codec_begin

struct ReconstructUtil
{
	static uint8_t Reconstruct(uint8_t predicted_val, int32_t residual_val);

	static BlockData<16, 16> Reconstruct(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<16, 16>& predicted_data);
	
	static BlockData<8, 8> Reconstruct(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<8, 8>& predicted_data);

};

__codec_end