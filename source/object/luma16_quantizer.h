#pragma once

#include <vector>

#include "block_data.h"

__codec_begin

class Luma16Quantizer
{
public:
	Luma16Quantizer(int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks);
	~Luma16Quantizer();

	void Quantize();

	BlockData<4, 4, int32_t> GetDCBlock() const;

	std::vector<BlockData<4, 4, int32_t>> GetACBlocks() const;

	bool IsACAllZero() const;

	uint32_t GetDetailedCBP() const;

private:
	int m_qp;
	BlockData<4, 4, int32_t> m_dc_block;
	std::vector<BlockData<4, 4, int32_t>> m_ac_blocks;
	bool m_is_ac_all_zero{ true };
	uint32_t m_detailed_cbp{ 0 };
};

__codec_end