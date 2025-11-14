#include "inter_p8x8_luma_flow_node_base.h"

#include "inter_p8x8_luma_flow_8x8_node.h"
#include "inter_p8x8_luma_flow_8x4_node.h"
#include "inter_p8x8_luma_flow_4x8_node.h"
#include "inter_p8x8_luma_flow_4x4_node.h"
#include "coding_util.h"
#include "encoder_context.h"

__codec_begin

InterP8x8LumaFlowNodeBase::InterP8x8LumaFlowNodeBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, MBType sub_mb_type) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index), m_sub_mb_type(sub_mb_type)
{
}

InterP8x8LumaFlowNodeBase::~InterP8x8LumaFlowNodeBase()
{
}

int64_t InterP8x8LumaFlowNodeBase::CalculateRDCost() const
{
	return CalculateDistortion() + m_encoder_context->lambda_motion_fp * CalculateRate();
}

uint32_t InterP8x8LumaFlowNodeBase::OutputSubMBTypes(std::shared_ptr<BytesData> bytes_data)
{
	return CodingUtil::UE_V((uint32_t)m_sub_mb_type - 4, bytes_data);
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
		node.reset(new InterP8x8LumaFlow8x4Node(mb, encoder_context, segment_index));
		break;
	case codec::MBType::SMB4x8:
		node.reset(new InterP8x8LumaFlow4x8Node(mb, encoder_context, segment_index));
		break;
	case codec::MBType::SMB4x4:
		node.reset(new InterP8x8LumaFlow4x4Node(mb, encoder_context, segment_index));
		break;
	default:
		break;
	}return node;
}

__codec_end