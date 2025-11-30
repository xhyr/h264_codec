#include "mb_inter_rd_optimizer.h"

#include <limits>

#include "macroblock.h"
#include "bytes_data.h"
#include "mb_inter_header_binaryer.h"
#include "inter_luma_flow_base.h"
#include "inter_chroma_flow.h"
#include "cost_util.h"
#include "encoder_context.h"
#include "cavlc_context.h"
#include "coding_util.h"

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

	int64_t min_rd_cost = std::numeric_limits<int64_t>::max();
	auto allowed_mb_types = {MBType::PSkip, MBType::P16x16, MBType::P16x8, MBType::P8x16, MBType::P8x8}; 
	MBType best_mb_type = *allowed_mb_types.begin();
	for (auto mb_type : allowed_mb_types)
	{
		m_mb_type = mb_type;
		m_mb->SetType(m_mb_type);

		m_luma_flow = InterLumaFlowBase::CreateFlow(mb_type, m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_flow->Backend();
		m_luma_cbp = m_luma_flow->GetCBP();

		m_chroma_flow = std::make_shared<InterChromaFlow>(m_mb, m_encoder_context, mb_type == MBType::PSkip);
		m_chroma_flow->Frontend();
		m_chroma_cbp = m_chroma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;

		int64_t distortion = m_luma_flow->GetSSEDistortion() + m_chroma_flow->GetSSEDistortion();
		auto tmp_bytes_data = std::make_shared<BytesData>();
		int64_t rate = Binary(tmp_bytes_data, true);

		auto rd_cost = CostUtil::CalculateRDCostMode(distortion, rate, m_encoder_context);
		if (rd_cost < min_rd_cost)
		{
			min_rd_cost = rd_cost;
			best_mb_type = mb_type;
		}
	}

	if (m_mb_type != best_mb_type)
	{
		m_mb_type = best_mb_type;
		m_mb->SetType(best_mb_type);
		m_luma_flow = InterLumaFlowBase::CreateFlow(best_mb_type, m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_flow->Backend();
		m_luma_cbp = m_luma_flow->GetCBP();

		m_chroma_flow = std::make_shared<InterChromaFlow>(m_mb, m_encoder_context, m_mb_type == MBType::PSkip);
		m_chroma_flow->Frontend();
		m_chroma_cbp = m_chroma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;
	}

	m_mb->SetLumaDetailedCBP(m_luma_flow->GetDetailedCBP());
	m_mb->SetReconstructedLumaBlockData(m_luma_flow->GetReconstructedData());
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cb), PlaneType::Cb);
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cr), PlaneType::Cr);
}

uint64_t MBInterRDOptimizer::Binary(std::shared_ptr<BytesData> bytes_data, bool rdo_pass)
{
	if (m_mb_type == MBType::PSkip && rdo_pass)
	{
		auto rate = CodingUtil::GetUEBitCount(m_encoder_context->mb_run + 1);
		rate -= CodingUtil::GetUEBitCount(m_encoder_context->mb_run);
		return rate;
	}

	auto start_bit_count = bytes_data->GetBitsCount();
	OutputMB(bytes_data, rdo_pass);
	uint64_t finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void MBInterRDOptimizer::OutputMB(std::shared_ptr<BytesData> bytes_data, bool rdo_pass)
{
	if (m_mb_type == MBType::PSkip)
	{
		++m_encoder_context->mb_run;

		if (m_mb_addr == m_encoder_context->mb_num - 1)
		{
			MBInterHeaderBinaryer header_binaryer(bytes_data);
			header_binaryer.OutputMBSkipRun(m_encoder_context->mb_run);
			m_encoder_context->mb_run = 0;
		}

		m_encoder_context->cavlc_context->ResetMBCoeffNums(m_mb_addr);

		return;
	}

	MBInterHeaderBinaryer header_binaryer(bytes_data);
	if (!rdo_pass)
	{
		header_binaryer.OutputMBSkipRun(m_encoder_context->mb_run);
		m_encoder_context->mb_run = 0;
	}
	header_binaryer.OutputMBType(m_mb_type);
	m_luma_flow->OutputSubMBTypes(bytes_data);
	m_luma_flow->OutputMotionInfo(bytes_data);
	header_binaryer.OutputCBP(m_cbp);
	if (m_cbp > 0)
		header_binaryer.OutputQPDelta(0);

	m_encoder_context->cavlc_context->ResetMBCoeffNums(m_mb->GetAddress());

	if(m_cbp & 0x0F)
		m_luma_flow->OutputCoefficients(bytes_data);
		
	if(m_cbp & 0x30)
		m_chroma_flow->OutputCoefficients(bytes_data);
}

__codec_end