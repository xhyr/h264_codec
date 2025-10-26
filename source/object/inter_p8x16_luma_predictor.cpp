#include "inter_p8x16_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"
#include "mb_util.h"
#include "full_search_util.h"

__codec_begin

InterP8x16LumaPredictor::InterP8x16LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
	Init();
}

InterP8x16LumaPredictor::~InterP8x16LumaPredictor()
{
}

void InterP8x16LumaPredictor::Decide()
{
	SearchInfo search_info{ m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0 };
	auto best_mv = FullSearchUtil::FindBestMV(search_info, m_encoder_context, m_mvd);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = MBUtil::GetOriginalLumaBlockData<8, 16>(m_mb, m_segment_index);
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<8, 16> InterP8x16LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

BlockData<8, 16, int32_t> InterP8x16LumaPredictor::GetDiffData() const
{
	return m_diff_data;
}

MotionInfo InterP8x16LumaPredictor::GetMotionInfo() const
{
	return m_motion_info;
}

MotionVector InterP8x16LumaPredictor::GetMVD() const
{
	return m_mvd;
}

void InterP8x16LumaPredictor::Init()
{
	auto pos = m_mb->GetPosition();
	m_x = pos.first + m_segment_index * 8;
	m_y = pos.second;
	m_x_in_block = pos.first / 4 + m_segment_index * 2;
	m_y_in_block = pos.second / 4;
	m_width_in_block = 2;
	m_height_in_block = 4;
}

__codec_end