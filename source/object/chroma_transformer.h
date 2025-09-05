#pragma once

#include "block_data.h"

__codec_begin

class ChromaTransformer
{
public:
	explicit ChromaTransformer(const BlockData<8, 8, int32_t>& block_data);
	~ChromaTransformer();

	void Transform();

	std::vector<BlockData<4, 4, int32_t>> GetBlocks() const;

	BlockData<2, 2, int32_t> GetDCBlock() const;

private:
	void PickBlocks();

	void TransformAll();

	void TransformDC();

private:
	BlockData<8, 8, int32_t> m_block_data;

	std::vector<BlockData<4, 4, int32_t>> m_blocks;
	BlockData<2, 2, int32_t> m_dc_block;
};

__codec_end