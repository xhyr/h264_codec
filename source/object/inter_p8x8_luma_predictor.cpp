#include "inter_p8x8_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"
#include "mb_util.h"
#include "full_search_util.h"
#include "motion_info_context.h"

__codec_begin

InterP8x8LumaPredictor::InterP8x8LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
	Init();
}

InterP8x8LumaPredictor::~InterP8x8LumaPredictor()
{
}

void InterP8x8LumaPredictor::Decide()
{
	SearchInfo search_info{ m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0 };
	auto best_mv = FullSearchUtil::FindBestMV(search_info, m_encoder_context, m_mvd);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = MBUtil::GetOriginalLumaBlockData<8, 8>(m_mb, m_segment_index); 
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<8, 8> InterP8x8LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

MotionVector InterP8x8LumaPredictor::GetMVD() const
{
	return m_mvd;
}

void InterP8x8LumaPredictor::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	diff_datas.resize(16);
	auto diff_block_datas = m_diff_data.GetTotalBlock4x4s();
	uint32_t index = 0;
	for (uint32_t y_in_block = (m_segment_index / 2) * 2; y_in_block < (m_segment_index / 2) * 2 + 2; ++y_in_block)
		for (uint32_t x_in_block = (m_segment_index % 2) * 2; x_in_block < (m_segment_index % 2) * 2 + 2; ++x_in_block)
			diff_datas[y_in_block * 4 + x_in_block] = diff_block_datas[index++];
}

void InterP8x8LumaPredictor::UpdateMotionInfo()
{
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), m_segment_index % 2 * 2, m_segment_index / 2 * 2, 2, 2, m_motion_info);
}

void InterP8x8LumaPredictor::Init()
{
	std::tie(m_x_in_block, m_y_in_block) = MBUtil::GetPositionInBlock(m_mb, m_segment_index);
	m_x = m_x_in_block << 2;
	m_y = m_y_in_block << 2;
	m_width_in_block = 2;
	m_height_in_block = 2;
}

__codec_end