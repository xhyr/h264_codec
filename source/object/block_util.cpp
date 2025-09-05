#include "block_util.h"

__codec_begin

uint32_t BlockUtil::CalculateBlockIndex(uint32_t block_8x8, uint32_t block_4x4)
{
	uint32_t x_in_block = block_8x8 % 2 * 2 + block_4x4 % 2;
	uint32_t y_in_block = block_8x8 / 2 * 2 + block_4x4 / 2;
	return x_in_block + y_in_block * 4;
}

__codec_end