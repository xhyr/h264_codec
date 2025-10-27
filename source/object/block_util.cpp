#include "block_util.h"

__codec_begin

uint32_t BlockUtil::CalculateBlockIndex(uint32_t block_8x8, uint32_t block_4x4)
{
	uint32_t x_in_block = block_8x8 % 2 * 2 + block_4x4 % 2;
	uint32_t y_in_block = block_8x8 / 2 * 2 + block_4x4 / 2;
	return x_in_block + y_in_block * 4;
}

std::vector<uint32_t> BlockUtil::GetBlockIndices(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, uint32_t frame_width_in_block)
{
	uint32_t frame_width_in_mb = frame_width_in_block / 4;
	uint32_t mb_offset_x_in_block = mb_addr % frame_width_in_mb * 4;
	uint32_t mb_offset_y_in_block = mb_addr / frame_width_in_mb * 4;

	std::vector<uint32_t> block_indices;
	block_indices.reserve(width_in_block * height_in_block);

	for (uint32_t i = y_in_block; i < y_in_block + height_in_block; ++i)
	{
		for (uint32_t j = x_in_block; j < x_in_block + width_in_block; ++j)
		{
			uint32_t block_index = (i + mb_offset_y_in_block) * frame_width_in_block + (j + mb_offset_x_in_block);
			block_indices.push_back(block_index);
		}
	}

	return block_indices;
}

bool BlockUtil::GetBlockNeighborIndex(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t frame_width_in_block, BlockNeighborType block_neighbor_type, uint32_t& block_index)
{
	auto got = true;
	switch (block_neighbor_type)
	{
		case codec::BlockNeighborType::A:
		{
			got = x_in_block > 0;
			if (got)
				block_index = GetBlockIndex(x_in_block - 1, y_in_block, frame_width_in_block);
			break;
		}
		case codec::BlockNeighborType::B:
		{
			got = y_in_block > 0;
			if (got)
				block_index = GetBlockIndex(x_in_block, y_in_block - 1, frame_width_in_block);
			break;
		}
		case codec::BlockNeighborType::C:
		{
			uint32_t new_x_in_block = x_in_block + width_in_block;
			got = new_x_in_block < frame_width_in_block && y_in_block > 0;
			if (got)
			{
				uint32_t new_y_in_block = y_in_block - 1;
				if (IsPreDecoded(new_x_in_block, new_y_in_block, x_in_block, y_in_block))
					block_index = GetBlockIndex(new_x_in_block, new_y_in_block, frame_width_in_block);
				else got = false;
			}
			break;
		}
		case codec::BlockNeighborType::D:
		{
			got = x_in_block > 0 && y_in_block > 0;
			if (got)
				block_index = GetBlockIndex(x_in_block - 1, y_in_block - 1, frame_width_in_block);
			break;
		}
		default:
			break;
	}

    return got;
}

uint32_t BlockUtil::GetBlockIndex(uint32_t x_in_block, uint32_t y_in_block, uint32_t frame_width_in_block)
{
	return x_in_block + y_in_block * frame_width_in_block;
}

uint32_t BlockUtil::IsPreDecoded(uint32_t new_x_in_block, uint32_t new_y_in_block, uint32_t x_in_block, uint32_t y_in_block)
{
	if (new_x_in_block == x_in_block && new_y_in_block == y_in_block)
		return false;

	uint32_t new_x_in_mb = new_x_in_block / 4;
	uint32_t new_y_in_mb = new_y_in_block / 4;

	uint32_t x_in_mb = x_in_block / 4;
	uint32_t y_in_mb = y_in_block / 4;

	if (new_y_in_mb < y_in_mb || (new_y_in_mb == y_in_mb && new_x_in_mb < x_in_mb))
		return true;
	else if (new_y_in_mb > y_in_mb || (new_y_in_mb == y_in_mb && new_x_in_mb > x_in_mb))
		return false;

	uint32_t new_x_in_block_8x8 = new_x_in_block / 2;
	uint32_t new_y_in_block_8x8 = new_y_in_block / 2;

	uint32_t x_in_block_8x8 = x_in_block / 2;
	uint32_t y_in_block_8x8 = y_in_block / 2;

	if (new_y_in_block_8x8 < y_in_block_8x8 || (new_y_in_block_8x8 == y_in_block_8x8 && new_x_in_block_8x8 < x_in_block_8x8))
		return true;
	else if (new_y_in_block_8x8 > y_in_block_8x8 || (new_y_in_block_8x8 == y_in_block_8x8 && new_x_in_block_8x8 > x_in_block_8x8))
		return false;

	if (new_y_in_block < y_in_block || (new_y_in_block == y_in_block && x_in_block < x_in_block_8x8))
		return true;
	else if (new_y_in_block > y_in_block || (new_y_in_block == y_in_block && x_in_block > x_in_block_8x8))
		return false;

	return false;
}

__codec_end