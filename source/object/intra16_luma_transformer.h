#pragma once

#include "block_data.h"

__codec_begin

class Intra16LumaTransformer
{
public:
	explicit Intra16LumaTransformer(const BlockData<16, 16, int32_t>& block_data);
	~Intra16LumaTransformer();

	void Transform();

	std::vector<BlockData<4, 4, int32_t>> GetBlocks() const;

	BlockData<4, 4, int32_t> GetDCBlock() const;

private:
	void PickBlocks();

	void TransformAll();

	void TransformDC();

private:
	BlockData<16, 16, int32_t> m_block_data;

	std::vector<BlockData<4, 4, int32_t>> m_blocks;
	BlockData<4, 4, int32_t> m_dc_block;
};

__codec_end