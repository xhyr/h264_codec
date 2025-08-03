#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class Intra16LumaQuantizer
{
public:
	Intra16LumaQuantizer(int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks);
	~Intra16LumaQuantizer();

	void Quantize();

	BlockData<4, 4, int32_t> GetDCBlock() const;

	std::vector<BlockData<4, 4, int32_t>> GetACBlocks() const;

	bool IsACAllZero() const;

private:
	int m_qp;
	BlockData<4, 4, int32_t> m_dc_block;
	std::vector<BlockData<4, 4, int32_t>> m_ac_blocks;
	bool m_is_ac_all_zero{ true };
};

__codec_end