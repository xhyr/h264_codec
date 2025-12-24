#include "inter_p8x4_luma_predictor.h"

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
#include "dpb.h"

__codec_begin

InterP8x4LumaPredictor::InterP8x4LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, uint8_t sub_segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index), m_sub_segment_index(sub_segment_index)
{
	Init();
}

InterP8x4LumaPredictor::~InterP8x4LumaPredictor()
{
}

void InterP8x4LumaPredictor::Decide()
{
	SearchInfo search_info{ m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0 };
	auto best_mv = FullSearchUtil::FindBestMV(search_info, m_encoder_context, m_mvd);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->dpb->GetFrame(0)->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = MBUtil::GetOriginalLumaBlockData<8, 4>(m_mb, m_segment_index, m_sub_segment_index);
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<8, 4> InterP8x4LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

MotionVector InterP8x4LumaPredictor::GetMVD() const
{
	return m_mvd;
}

void InterP8x4LumaPredictor::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	diff_datas.resize(16);
	auto diff_block_datas = m_diff_data.GetTotalBlock4x4s();

	for(uint8_t i = 0; i < 2; ++i)
		diff_datas[m_x_in_block_mb + i + m_y_in_block_mb * 4] = diff_block_datas[i];
}

void InterP8x4LumaPredictor::UpdateMotionInfo()
{
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), m_x_in_block_mb, m_y_in_block_mb, m_width_in_block, m_height_in_block, m_motion_info);
}

void InterP8x4LumaPredictor::FillPredictedData(BlockData<8, 8>& predicted_data) const
{
	for(uint8_t i = 0; i < 2; ++i)
		predicted_data.SetBlock4x4(i, m_sub_segment_index, m_predicted_data.GetBlock4x4<uint8_t>(i, 0));
}

void InterP8x4LumaPredictor::Init()
{
	std::tie(m_x_in_block, m_y_in_block) = MBUtil::GetPositionInBlock(m_mb, m_segment_index);
	m_y_in_block += m_sub_segment_index;
	m_x_in_block_mb = m_x_in_block % 4;
	m_y_in_block_mb = m_y_in_block % 4;
	m_x = m_x_in_block << 2;
	m_y = m_y_in_block << 2;
	m_width_in_block = 2;
	m_height_in_block = 1;
}

__codec_end