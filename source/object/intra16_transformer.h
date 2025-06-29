#pragma once

#include "block_data.h"

__codec_begin

class Intra16Transformer
{
public:
	explicit Intra16Transformer(const BlockData<16, 16, int>& block_data);
	~Intra16Transformer();

	void Transform();

	std::vector<BlockData<4, 4, int>> GetBlocks() const;

	BlockData<4, 4, int> GetDCBlock() const;

private:
	void PickBlocks();

	void TransformAll();

	void TransformDC();

private:
	BlockData<16, 16, int> m_block_data;

	std::vector<BlockData<4, 4, int>> m_blocks;
	BlockData<4, 4, int> m_dc_block;
};

__codec_end