#include "inter_p8x8_luma_flow_node_base.h"

#include "inter_p8x8_luma_flow_8x8_node.h"

__codec_begin

InterP8x8LumaFlowNodeBase::InterP8x8LumaFlowNodeBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
}

InterP8x8LumaFlowNodeBase::~InterP8x8LumaFlowNodeBase()
{
}

BlockData<8, 8> InterP8x8LumaFlowNodeBase::GetPredictedData() const
{
    return m_predicted_data;
}

std::shared_ptr<InterP8x8LumaFlowNodeBase> InterP8x8LumaFlowNodeBase::Create(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index)
{
	std::shared_ptr<InterP8x8LumaFlowNodeBase> node;
	switch (mb_type)
	{
	case codec::MBType::SMB8x8:
		node.reset(new InterP8x8LumaFlow8x8Node(mb, encoder_context, segment_index));
		break;
	case codec::MBType::SMB8x4:
		break;
	case codec::MBType::SMB48x8:
		break;
	case codec::MBType::SMB4x4:
		break;
	default:
		break;
	}return node;
}

__codec_end