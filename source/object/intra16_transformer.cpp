#include "intra16_transformer.h"

#include <algorithm>

#include "transform_util.h"

__codec_begin

Intra16Transformer::Intra16Transformer(const BlockData<16, 16, int>& block_data) : m_block_data(block_data)
{
}

Intra16Transformer::~Intra16Transformer()
{
}

void Intra16Transformer::Transform()
{
	PickBlocks();
	TransformAll();
	TransformDC();
}

std::vector<BlockData<4, 4, int>> Intra16Transformer::GetBlocks() const
{
	return m_blocks;
}

BlockData<4, 4, int> Intra16Transformer::GetDCBlock() const
{
	return m_dc_block;
}

void Intra16Transformer::PickBlocks()
{
	m_blocks.reserve(16);
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
		for (uint32_t x_in_block = 0; x_in_block < 4; ++x_in_block)	
			m_blocks.emplace_back(m_block_data.GetBlock4x4(x_in_block, y_in_block));
}

void Intra16Transformer::TransformAll()
{
	std::transform(m_blocks.begin(), m_blocks.end(), m_blocks.begin(), &TransformUtil::DCT);	
}

void Intra16Transformer::TransformDC()
{
	for (uint32_t y = 0; y < 4; ++y)
		for (uint32_t x = 0; x < 4; ++x)
			m_dc_block.SetElement(x, y, m_blocks[y * 4 + x].GetElement(0, 0));

	m_dc_block = TransformUtil::Hadamard(m_dc_block);
}

__codec_end