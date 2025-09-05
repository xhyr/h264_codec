#include "chroma_transformer.h"

#include <algorithm>

#include "transform_util.h"

__codec_begin

ChromaTransformer::ChromaTransformer(const BlockData<8, 8, int32_t>& block_data) : m_block_data(block_data)
{
}

ChromaTransformer::~ChromaTransformer()
{
}

void ChromaTransformer::Transform()
{
	PickBlocks();
	TransformAll();
	TransformDC();
}

std::vector<BlockData<4, 4, int32_t>> ChromaTransformer::GetBlocks() const
{
	return m_blocks;
}

BlockData<2, 2, int32_t> ChromaTransformer::GetDCBlock() const
{
	return m_dc_block;
}

void ChromaTransformer::PickBlocks()
{
	m_blocks.reserve(4);
	for (uint32_t y_in_block = 0; y_in_block < 2; ++y_in_block)
		for (uint32_t x_in_block = 0; x_in_block < 2; ++x_in_block)
			m_blocks.emplace_back(m_block_data.GetBlock4x4(x_in_block, y_in_block));
}

void ChromaTransformer::TransformAll()
{
	std::transform(m_blocks.begin(), m_blocks.end(), m_blocks.begin(), &TransformUtil::DCT);
}

void ChromaTransformer::TransformDC()
{
	for (uint32_t y = 0; y < 2; ++y)
		for (uint32_t x = 0; x < 2; ++x)
			m_dc_block.SetElement(x, y, m_blocks[y * 2 + x].GetElement(0, 0));

	m_dc_block = TransformUtil::Hadamard(m_dc_block);
}

__codec_end