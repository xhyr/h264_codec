#include "intra16_luma_quantizer.h"

#include "quantize_util.h"

__codec_begin

Intra16LumaQuantizer::Intra16LumaQuantizer(int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks) :
	m_qp(qp), m_dc_block(dc_block), m_ac_blocks(ac_blocks)
{
}

Intra16LumaQuantizer::~Intra16LumaQuantizer()
{
}

void Intra16LumaQuantizer::Quantize()
{
	m_dc_block = QuantizeUtil::QuantizeDC(m_qp, m_dc_block);
	for (auto& ac_block : m_ac_blocks)
		ac_block = QuantizeUtil::QuantizeAC(m_qp, ac_block);
}

BlockData<4, 4, int32_t> Intra16LumaQuantizer::GetDCBlock() const
{
	return m_dc_block;
}

std::vector<BlockData<4, 4, int32_t>> Intra16LumaQuantizer::GetACBlocks() const
{
	return m_ac_blocks;
}

__codec_end