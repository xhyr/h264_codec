#include "intra16_quantizer.h"

#include "quantize_util.h"

__codec_begin

Intra16Quantizer::Intra16Quantizer(int qp, const BlockData<4, 4, int>& dc_block, const std::vector<BlockData<4, 4, int>>& ac_blocks) :
	m_qp(qp), m_dc_block(dc_block), m_ac_blocks(ac_blocks)
{
}

Intra16Quantizer::~Intra16Quantizer()
{
}

void Intra16Quantizer::Quantize()
{
	m_dc_block = QuantizeUtil::QuantizeDC(m_qp, m_dc_block);
	for (auto& ac_block : m_ac_blocks)
		ac_block = QuantizeUtil::QuantizeAC(m_qp, ac_block);
}

BlockData<4, 4, int> Intra16Quantizer::GetDCBlock() const
{
	return m_dc_block;
}

std::vector<BlockData<4, 4, int>> Intra16Quantizer::GetACBlocks() const
{
	return m_ac_blocks;
}

__codec_end