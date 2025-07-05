#include "intra8_chroma_quantizer.h"

#include "quantize_util.h"

__codec_begin

Intra8ChromaQuantizer::Intra8ChromaQuantizer(int qp, const BlockData<2, 2, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>>& ac_blocks)
{
}

Intra8ChromaQuantizer::~Intra8ChromaQuantizer()
{
}

void Intra8ChromaQuantizer::Quantize()
{
	m_dc_block = QuantizeUtil::QuantizeDC(m_qp, m_dc_block);
	for (auto& ac_block : m_ac_blocks)
		ac_block = QuantizeUtil::QuantizeAC(m_qp, ac_block);
}

BlockData<2, 2, int32_t> Intra8ChromaQuantizer::GetDCBlock() const
{
	return m_dc_block;
}

std::vector<BlockData<4, 4, int32_t>> Intra8ChromaQuantizer::GetACBlocks() const
{
	return m_ac_blocks;
}

__codec_end