#include "inter_p16x8_luma_predictor.h"

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

InterP16x8LumaPredictor::InterP16x8LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
	Init();
}

InterP16x8LumaPredictor::~InterP16x8LumaPredictor()
{
}

void InterP16x8LumaPredictor::Decide()
{
	SearchInfo search_info{ m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0 };
	auto best_mv = FullSearchUtil::FindBestMV(search_info, m_encoder_context, m_mvd);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = MBUtil::GetOriginalLumaBlockData<16, 8>(m_mb, m_segment_index);  
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<16, 8> InterP16x8LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

MotionVector InterP16x8LumaPredictor::GetMVD() const
{
	return m_mvd;
}

void InterP16x8LumaPredictor::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	diff_datas.resize(16);
	auto diff_block_datas = m_diff_data.GetTotalBlock4x4s();
	for (uint8_t i = m_segment_index * 8; i < m_segment_index * 8 + 8; ++i)
		diff_datas[i] = diff_block_datas[i - m_segment_index * 8];
}

void InterP16x8LumaPredictor::UpdateMotionInfo()
{
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, m_motion_info);
}

void InterP16x8LumaPredictor::Init()
{
	auto pos = m_mb->GetPosition();
	m_x = pos.first;
	m_y = pos.second + m_segment_index * 8;
	m_x_in_block = pos.first / 4;
	m_y_in_block = pos.second / 4 + m_segment_index * 2;
	m_width_in_block = 4;
	m_height_in_block = 2;
}

__codec_end