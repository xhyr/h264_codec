#include "intra_luma_flow_base.h"

__codec_begin

IntraLumaFlowBase::IntraLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

IntraLumaFlowBase::~IntraLumaFlowBase()
{
}

BlockData<16, 16> IntraLumaFlowBase::GetReconstructedData() const
{
	return m_reconstructed_data;
}

int IntraLumaFlowBase::GetCost() const
{
	return m_cost;
}

uint8_t IntraLumaFlowBase::GetCBP() const
{
	return m_cbp;
}

__codec_end