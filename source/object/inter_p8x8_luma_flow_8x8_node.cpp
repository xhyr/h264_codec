#include "inter_p8x8_luma_flow_8x8_node.h"

#include "inter_p8x8_luma_predictor.h"
#include "bytes_data.h"
#include "coding_util.h"

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
	m_predictor->UpdateMotionInfo();
	m_predicted_data = m_predictor->GetPredictedData();
}

void InterP8x8LumaFlow8x8Node::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	m_predictor->FillDiffData(diff_datas);
}

void InterP8x8LumaFlow8x8Node::UpdateMotionInfo()
{
	m_predictor->UpdateMotionInfo();
}

uint32_t InterP8x8LumaFlow8x8Node::OutputMotionInfos(std::shared_ptr<BytesData> bytes_data) const
{
	auto start_bit_count = bytes_data->GetBitsCount();

	auto mvd = m_predictor->GetMVD();

	CodingUtil::SE_V(mvd.x, bytes_data);
	CodingUtil::SE_V(mvd.y, bytes_data);

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void InterP8x8LumaFlow8x8Node::Init()
{
	m_predictor = std::make_unique<InterP8x8LumaPredictor>(m_mb, m_encoder_context, m_segment_index);
}

__codec_end