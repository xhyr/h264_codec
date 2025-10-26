#include "inter_p8x8_luma_flow_node_base.h"

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

BlockData<8, 8, int32_t> InterP8x8LumaFlowNodeBase::GetDiffData() const
{
	return m_diff_data;
}

__codec_end