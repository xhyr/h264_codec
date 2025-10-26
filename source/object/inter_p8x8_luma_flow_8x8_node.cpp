#include "inter_p8x8_luma_flow_8x8_node.h"

#include "inter_p8x8_luma_predictor.h"

__codec_begin

InterP8x8LumaFlow8x8Node::InterP8x8LumaFlow8x8Node(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	InterP8x8LumaFlowNodeBase(mb, encoder_context, segment_index)
{
	Init();
}

InterP8x8LumaFlow8x8Node::~InterP8x8LumaFlow8x8Node()
{
}

void InterP8x8LumaFlow8x8Node::Predict()
{
	m_predictor->Decide();
}

std::vector<MotionInfo> InterP8x8LumaFlow8x8Node::GetMotionInfos() const
{
	return { m_predictor->GetMotionInfo() };
}

std::vector<MotionVector> InterP8x8LumaFlow8x8Node::GetMVDs() const
{
	return { m_predictor->GetMVD() };
}

void InterP8x8LumaFlow8x8Node::Init()
{
	m_predictor = std::make_unique<InterP8x8LumaPredictor>(m_mb, m_encoder_context, m_segment_index);
}

__codec_end