#include "inter_p8x8_luma_flow_8x4_node.h"

#include "inter_p8x4_luma_predictor.h"
#include "bytes_data.h"
#include "coding_util.h"
#include "macroblock.h"

__codec_begin

InterP8x8LumaFlow8x4Node::InterP8x8LumaFlow8x4Node(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	InterP8x8LumaFlowNodeBase(mb, encoder_context, segment_index, MBType::SMB8x4)
{
	Init();
}

InterP8x8LumaFlow8x4Node::~InterP8x8LumaFlow8x4Node()
{
}

void InterP8x8LumaFlow8x4Node::Predict()
{
	for (uint8_t i = 0; i < 2; ++i)
	{
		m_predictors[i]->Decide();
		m_predictors[i]->UpdateMotionInfo();
		m_predictors[i]->FillPredictedData(m_predicted_data);
	}
}

void InterP8x8LumaFlow8x4Node::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	for (uint8_t i = 0; i < 2; ++i)
		m_predictors[i]->FillDiffData(diff_datas);
}

void InterP8x8LumaFlow8x4Node::UpdateMotionInfo()
{
	for (uint8_t i = 0; i < 2; ++i)
		m_predictors[i]->UpdateMotionInfo();
}

uint32_t InterP8x8LumaFlow8x4Node::OutputMotionInfos(std::shared_ptr<BytesData> bytes_data) const
{
	auto start_bit_count = bytes_data->GetBitsCount();

	for (uint8_t i = 0; i < 2; ++i)
	{
		auto mvd = m_predictors[i]->GetMVD();

		CodingUtil::SE_V(mvd.x, bytes_data);
		CodingUtil::SE_V(mvd.y, bytes_data);
	}
	
	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void InterP8x8LumaFlow8x4Node::Init()
{
	for(uint8_t sub_segment_index = 0; sub_segment_index < 2; ++sub_segment_index)
		m_predictors[sub_segment_index] = std::make_unique<InterP8x4LumaPredictor>(m_mb, m_encoder_context, m_segment_index, sub_segment_index);
}

__codec_end