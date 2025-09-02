#include "inter_luma_flow_base.h"

#include "macroblock.h"
#include "cost_util.h"

__codec_begin

InterLumaFlowBase::InterLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

InterLumaFlowBase::~InterLumaFlowBase()
{
}

BlockData<16, 16> InterLumaFlowBase::GetReconstructedData() const
{
	return m_reconstructed_data;
}

int InterLumaFlowBase::GetDistortion() const
{
	return m_distortion;
}

uint8_t InterLumaFlowBase::GetCBP() const
{
	return m_cbp;
}

void InterLumaFlowBase::CalculateDistortion()
{
	auto original_block_data = m_mb->GetOriginalLumaBlockData16x16();
	m_distortion = CostUtil::CalculateSADDistortion(original_block_data, m_reconstructed_data);
}

__codec_end