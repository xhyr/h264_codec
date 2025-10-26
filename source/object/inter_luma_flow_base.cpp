#include "inter_luma_flow_base.h"

#include "macroblock.h"
#include "cost_util.h"
#include "inter_p16x16_luma_flow.h"
#include "inter_p16x8_luma_flow.h"
#include "inter_p8x16_luma_flow.h"

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

uint32_t InterLumaFlowBase::GetDetailedCBP() const
{
    return m_detailed_cbp;
}

void InterLumaFlowBase::CalculateDistortion()
{
	auto original_block_data = m_mb->GetOriginalLumaBlockData();
	m_distortion = CostUtil::CalculateSADDistortion(original_block_data, m_reconstructed_data);
}

std::shared_ptr<InterLumaFlowBase> InterLumaFlowBase::CreateFlow(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context)
{
	std::shared_ptr<InterLumaFlowBase> flow;
	switch (mb_type)
	{
	case codec::MBType::PSkip:
		break;
	case codec::MBType::P16x16:
		flow.reset(new InterP16x16LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::P16x8:
		flow.reset(new InterP16x8LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::P8x16:
		flow.reset(new InterP8x16LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::I4:
		break;
	case codec::MBType::I16:
		break;
	default:
		break;
	}
	return flow;
}

__codec_end