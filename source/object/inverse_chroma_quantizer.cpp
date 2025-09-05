#include "inverse_chroma_quantizer.h"

#include "quantize_util.h"

__codec_begin

InverseChromaQuantizer::InverseChromaQuantizer(int qp, const BlockData<2, 2, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks) :
	m_qp(qp), m_dc_block(dc_block), m_blocks(ac_blocks)
{
}

InverseChromaQuantizer::~InverseChromaQuantizer()
{
}

void InverseChromaQuantizer::InverseQuantize()
{
	m_dc_block = QuantizeUtil::InverseQuantizeDC(m_qp, m_dc_block);

	for (uint32_t i = 0; i < 4; ++i)
	{
		m_blocks[i] = QuantizeUtil::InverseQuantizeAC(m_qp, m_blocks[i]);
		m_blocks[i].SetElement(0, 0, m_dc_block.GetElement(i % 2, i / 2));
	}
}

std::vector<BlockData<4, 4, int32_t>> InverseChromaQuantizer::GetBlocks() const
{
	return m_blocks;
}

__codec_end