#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class ChromaQuantizer
{
public:
	ChromaQuantizer(bool is_slice_intra, int qp, const BlockData<2, 2, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks, bool is_intra = true);
	~ChromaQuantizer();

	void Quantize();

	BlockData<2, 2, int32_t> GetDCBlock() const;

	std::vector<BlockData<4, 4, int32_t>> GetACBlocks() const;

	void ResetACToZeros();

private:
	int m_qp;
	BlockData<2, 2, int32_t> m_dc_block;
	std::vector<BlockData<4, 4, int32_t>> m_ac_blocks;
	bool m_is_intra;
};

__codec_end