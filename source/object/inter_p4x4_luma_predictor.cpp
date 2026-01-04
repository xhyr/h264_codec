#include "inter_p4x4_luma_predictor.h"

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

InterP4x4LumaPredictor::InterP4x4LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, uint8_t sub_segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index), m_sub_segment_index(sub_segment_index)
{
	Init();
}

InterP4x4LumaPredictor::~InterP4x4LumaPredictor()
{
}

void InterP4x4LumaPredictor::Decide()
{
	SearchInfo search_info{ m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block };
	m_motion_info = FullSearchUtil::FindBestMotionInfo(search_info, m_encoder_context, m_mvd);

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->dpb->GetFrame(m_motion_info.ref_id)->y_data, m_x + m_motion_info.mv.x / 4, m_y + m_motion_info.mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = MBUtil::GetOriginalLumaBlockData<4, 4>(m_mb, m_segment_index, m_sub_segment_index);
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<4, 4> InterP4x4LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

MotionVector InterP4x4LumaPredictor::GetMVD() const
{
	return m_mvd;
}

void InterP4x4LumaPredictor::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	diff_datas.resize(16);
	diff_datas[m_x_in_block_mb + m_y_in_block_mb * 4] = m_diff_data;
}

void InterP4x4LumaPredictor::UpdateMotionInfo()
{
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), m_x_in_block_mb, m_y_in_block_mb, m_width_in_block, m_height_in_block, m_motion_info);
}

void InterP4x4LumaPredictor::FillPredictedData(BlockData<8, 8>& predicted_data) const
{
	predicted_data.SetBlock4x4(m_sub_segment_index % 2, m_sub_segment_index / 2, m_predicted_data);
}

void InterP4x4LumaPredictor::Init()
{
	std::tie(m_x_in_block, m_y_in_block) = MBUtil::GetPositionInBlock(m_mb, m_segment_index);
	m_x_in_block += m_sub_segment_index % 2;
	m_y_in_block += m_sub_segment_index / 2;
	m_x_in_block_mb = m_x_in_block % 4;
	m_y_in_block_mb = m_y_in_block % 4;
	m_x = m_x_in_block << 2;
	m_y = m_y_in_block << 2;
	m_width_in_block = 1;
	m_height_in_block = 1;
}

__codec_end