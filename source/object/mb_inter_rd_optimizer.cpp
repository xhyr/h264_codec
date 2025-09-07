#include "mb_inter_rd_optimizer.h"

#include <limits>

#include "macroblock.h"
#include "inter_p16x16_luma_flow.h"
#include "inter_p16x16_chroma_flow.h"
#include "bytes_data.h"
#include "mb_inter_header_binaryer.h"

__codec_begin

MBInterRDOptimizer::MBInterRDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

MBInterRDOptimizer::~MBInterRDOptimizer()
{
}

void MBInterRDOptimizer::Encode()
{
	m_rd_cost = std::numeric_limits<int>::max();
	m_mb_addr = m_mb->GetAddress();

	if (m_mb_addr == 5)
		int sb = 1;

	m_luma_flow.reset(new InterP16x16LumaFlow(m_mb, m_encoder_context));
	m_luma_flow->Frontend();
	m_best_luma_flow = m_luma_flow;
	m_luma_cbp = m_luma_flow->GetCBP();

	m_chroma_flow.reset(new InterP16x16ChromaFlow(m_mb, m_encoder_context));
	m_chroma_flow->Frontend();
	m_chroma_cbp = m_chroma_flow->GetCBP();
	m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;

	m_mb_type = MBType::P16x16;
}

uint32_t MBInterRDOptimizer::Binary(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();
	OutputMB(bytes_data);
	uint32_t finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void MBInterRDOptimizer::OutputMB(std::shared_ptr<BytesData> bytes_data)
{
	if (m_mb_addr == 6)
		int sb = 1;

	MBInterHeaderBinaryer header_binaryer(bytes_data);
	header_binaryer.OutputMBSkipRun(0);
	header_binaryer.OutputMBType(m_mb_type);
	m_best_luma_flow->OutputMotionInfo(bytes_data);
	header_binaryer.OutputCBP(m_cbp);
	if (m_cbp > 0)
		header_binaryer.OutputQPDelta(0);

	if(m_cbp & 0x0F)
		m_luma_flow->OutputCoefficients(bytes_data);

	if(m_cbp & 0x30)
		m_chroma_flow->OutputCoefficients(bytes_data);
}

__codec_end