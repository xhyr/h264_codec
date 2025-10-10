#include "inter_chroma_flow_base.h"

#include "chroma_quantizer.h"
#include "inter_p16x16_chroma_flow.h"
#include "inter_p16x8_chroma_flow.h"

__codec_begin

InterChromaFlowBase::InterChromaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

InterChromaFlowBase::~InterChromaFlowBase()
{
}

BlockData<8, 8> InterChromaFlowBase::GetReconstructedData(PlaneType plane_type)
{
	return m_reconstructed_data_map[plane_type];
}

uint8_t InterChromaFlowBase::GetCBP() const
{
	return m_cbp;
}

int InterChromaFlowBase::GetDistortion() const
{
	return m_distortion;
}

std::shared_ptr<InterChromaFlowBase> InterChromaFlowBase::CreateFlow(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context)
{
	std::shared_ptr<InterChromaFlowBase> flow;
	switch (mb_type)
	{
	case codec::MBType::PSkip:
		break;
	case codec::MBType::P16x16:
		flow.reset(new InterP16x16ChromaFlow(mb, encoder_context));
		break;
	case codec::MBType::P16x8:
		flow.reset(new InterP16x8ChromaFlow(mb, encoder_context));
		break;
	case codec::MBType::P8x16:
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