#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class InverseIntra16Quantizer
{
public:
	InverseIntra16Quantizer(int qp, const BlockData<4, 4, int>& dc_block, const std::vector<BlockData<4, 4, int>>& ac_blocks);
	~InverseIntra16Quantizer();

	void InverseQuantize();

	std::vector<BlockData<4, 4, int>> GetBlocks() const;

private:
	int m_qp;
	BlockData<4, 4, int> m_dc_block;
	std::vector<BlockData<4, 4, int>> m_blocks;
};

__codec_end