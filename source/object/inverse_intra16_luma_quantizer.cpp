#include "inverse_intra16_luma_quantizer.h"

#include "quantize_util.h"

__codec_begin

InverseIntra16LumaQuantizer::InverseIntra16LumaQuantizer(int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks) :
	m_qp(qp), m_dc_block(dc_block), m_blocks(ac_blocks)
{
}

InverseIntra16LumaQuantizer::~InverseIntra16LumaQuantizer()
{
}

void InverseIntra16LumaQuantizer::InverseQuantize()
{
	m_dc_block = QuantizeUtil::InverseQuantizeDC(m_qp, m_dc_block);

	for (uint32_t i = 0; i < 16; ++i)
	{
		m_blocks[i] = QuantizeUtil::InverseQuantizeAC(m_qp, m_blocks[i]);
		m_blocks[i].SetElement(0, 0, m_dc_block.GetElement(i % 4, i / 4));
	}
}

std::vector<BlockData<4, 4, int32_t>> InverseIntra16LumaQuantizer::GetBlocks() const
{
	return m_blocks;
}

__codec_end