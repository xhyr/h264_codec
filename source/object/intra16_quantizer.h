#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class Intra16Quantizer
{
public:
	Intra16Quantizer(int qp, const BlockData<4, 4, int>& dc_block, const std::vector<BlockData<4, 4, int>>& ac_blocks);
	~Intra16Quantizer();

	void Quantize();

	BlockData<4, 4, int> GetDCBlock() const;

	std::vector<BlockData<4, 4, int>> GetACBlocks() const;

private:
	int m_qp;
	BlockData<4, 4, int> m_dc_block;
	std::vector<BlockData<4, 4, int>> m_ac_blocks;
};

__codec_end