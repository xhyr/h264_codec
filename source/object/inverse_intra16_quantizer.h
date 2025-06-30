#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class InverseIntra16Quantizer
{
public:
	InverseIntra16Quantizer(int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks);
	~InverseIntra16Quantizer();

	void InverseQuantize();

	std::vector<BlockData<4, 4, int32_t>> GetBlocks() const;

private:
	int m_qp;
	BlockData<4, 4, int32_t> m_dc_block;
	std::vector<BlockData<4, 4, int32_t>> m_blocks;
};

__codec_end