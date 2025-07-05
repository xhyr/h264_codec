#pragma once

#include <cstdint>
#include <vector>

#include "block_data.h"

__codec_begin

class Intra16LumaReconstructor
{
public:
	Intra16LumaReconstructor(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<16, 16>& predicted_data);
	~Intra16LumaReconstructor();

	void Reconstruct();

	BlockData<16, 16> GetBlockData() const;

private:
	std::vector<BlockData<4, 4, int32_t>> m_residual_blocks;
	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16> m_block_data;
};

__codec_end