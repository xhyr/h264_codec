#include "inter_pskip_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"
#include "motion_info_context.h"
#include "dpb.h"

__codec_begin

InterPSkipLumaPredictor::InterPSkipLumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
	Init();
}

InterPSkipLumaPredictor::~InterPSkipLumaPredictor()
{
}

void InterPSkipLumaPredictor::Decide()
{
	auto best_mv = MEUtil::GetPredictorMVPSkip(m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0, m_encoder_context->motion_info_context);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;
	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->dpb->GetFrame(0)->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
}

BlockData<16, 16> InterPSkipLumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

MotionVector InterPSkipLumaPredictor::GetMVD() const
{
    return m_mvd;
}

void InterPSkipLumaPredictor::FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const
{
	for (auto& diff_data : diff_datas)
		diff_data.Reset(0);
}

void InterPSkipLumaPredictor::UpdateMotionInfo()
{
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), m_x_in_block_mb, m_y_in_block_mb, m_width_in_block, m_height_in_block, m_motion_info);
}

void InterPSkipLumaPredictor::Init()
{
	auto pos = m_mb->GetPosition();
	m_x = pos.first;
	m_y = pos.second;
	m_x_in_block = pos.first / 4;
	m_y_in_block = pos.second / 4;
	m_x_in_block_mb = m_x_in_block % 4;
	m_y_in_block_mb = m_y_in_block % 4;
	m_width_in_block = 4;
	m_height_in_block = 4;
}

__codec_end