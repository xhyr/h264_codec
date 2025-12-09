#include "chroma_quantizer.h"

#include "quantize_util.h"

__codec_begin

ChromaQuantizer::ChromaQuantizer(bool is_slice_intra, int qp, const BlockData<2, 2, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks, bool is_intra) :
	m_is_slice_intra(is_slice_intra), m_qp(qp), m_dc_block(dc_block), m_ac_blocks(ac_blocks)
{
}

ChromaQuantizer::~ChromaQuantizer()
{
}

void ChromaQuantizer::Quantize()
{
	m_dc_block = QuantizeUtil::QuantizeDC(m_qp, m_dc_block, m_is_slice_intra);
	for (auto& ac_block : m_ac_blocks)
		ac_block = QuantizeUtil::QuantizeAC(m_qp, ac_block, m_is_slice_intra);
}

BlockData<2, 2, int32_t> ChromaQuantizer::GetDCBlock() const
{
	return m_dc_block;
}

std::vector<BlockData<4, 4, int32_t>> ChromaQuantizer::GetACBlocks() const
{
	return m_ac_blocks;
}

void ChromaQuantizer::ResetACToZeros()
{
	for (auto& ac_block : m_ac_blocks)
		ac_block.Reset(0);
}

__codec_end