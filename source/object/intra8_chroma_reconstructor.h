#pragma once

#include <cstdint>
#include <vector>

#include "block_data.h"

__codec_begin

class Intra8ChromaReconstructor
{
public:
	Intra8ChromaReconstructor(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<8, 8>& predicted_data);
	~Intra8ChromaReconstructor();

	void Reconstruct();

	BlockData<8, 8> GetBlockData() const;

private:
	std::vector<BlockData<4, 4, int32_t>> m_residual_blocks;
	BlockData<8, 8> m_predicted_data;
	BlockData<8, 8> m_block_data;
};

__codec_end