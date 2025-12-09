#include "luma16_quantizer.h"

#include "quantize_util.h"

__codec_begin

Luma16Quantizer::Luma16Quantizer(bool is_slice_intra, int qp, const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks) :
	m_is_slice_intra(is_slice_intra), m_qp(qp), m_dc_block(dc_block), m_ac_blocks(ac_blocks)
{
}

Luma16Quantizer::~Luma16Quantizer()
{
}

void Luma16Quantizer::Quantize()
{
	m_dc_block = QuantizeUtil::QuantizeDC(m_qp, m_dc_block, m_is_slice_intra);
	for (auto& ac_block : m_ac_blocks)
	{
		ac_block = QuantizeUtil::QuantizeAC(m_qp, ac_block, m_is_slice_intra);
		bool all_zero = ac_block.AllEqual(0);
		m_is_ac_all_zero = m_is_ac_all_zero && all_zero;
		m_detailed_cbp = (m_detailed_cbp << 1) | !all_zero;
	}
}

BlockData<4, 4, int32_t> Luma16Quantizer::GetDCBlock() const
{
	return m_dc_block;
}

std::vector<BlockData<4, 4, int32_t>> Luma16Quantizer::GetACBlocks() const
{
	return m_ac_blocks;
}

bool Luma16Quantizer::IsACAllZero() const
{
	return m_is_ac_all_zero;
}

uint32_t Luma16Quantizer::GetDetailedCBP() const
{
	return m_detailed_cbp;
}

__codec_end