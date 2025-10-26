#include "intra_luma_flow_base.h"

#include "macroblock.h"
#include "cost_util.h"

__codec_begin

IntraLumaFlowBase::IntraLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

IntraLumaFlowBase::~IntraLumaFlowBase()
{
}

void IntraLumaFlowBase::OutputPredictionTypes(std::shared_ptr<BytesData> bytes_data)
{
}

BlockData<16, 16> IntraLumaFlowBase::GetReconstructedData() const
{
	return m_reconstructed_data;
}

int IntraLumaFlowBase::GetDistortion() const
{
	return m_distortion;
}

uint8_t IntraLumaFlowBase::GetCBP() const
{
	return m_cbp;
}

uint32_t IntraLumaFlowBase::GetDetailedCBP() const
{
	return m_detailed_cbp;
}

void IntraLumaFlowBase::CalculateDistortion()
{
	auto original_block_data = m_mb->GetOriginalLumaBlockData();
	m_distortion = CostUtil::CalculateSADDistortion(original_block_data, m_reconstructed_data);
}

__codec_end