#include "inter_p16x16_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"

__codec_begin

InterP16x16LumaPredictor::InterP16x16LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
	Init();
}

InterP16x16LumaPredictor::~InterP16x16LumaPredictor()
{
}

void InterP16x16LumaPredictor::Decide()
{
	auto pred_mv = MEUtil::GetPredictorMV(m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, 0, m_encoder_context->motion_info_context);

	int min_cost = std::numeric_limits<int>::max();
	MotionVector best_mv;
	for (auto mv : m_encoder_context->search_motion_vectors)
	{
		auto cand_mv = pred_mv + mv;
		auto cost = MEUtil::CalculateMVCost(pred_mv, cand_mv, m_encoder_context->lambda_motion_fp);
		auto full_pixel_mv = MotionVector{ cand_mv.x / 4, cand_mv.y / 4 };
		cost += CostUtil::CalculateLumaSAD(m_x_in_block, m_y_in_block, m_width_in_block, m_height_in_block, m_encoder_context->yuv_frame, m_encoder_context->last_frame, full_pixel_mv);

		if (cost < min_cost)
		{
			min_cost = cost;
			best_mv = cand_mv;
		}
	}

	best_mv = MEUtil::ClipMVRange(best_mv, m_encoder_context);
	m_motion_info.ref_id = 0;
	m_motion_info.mv = best_mv;
	m_mvd = best_mv - pred_mv;

	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, m_x + best_mv.x / 4, m_y + best_mv.y / 4, m_width_in_block * 4, m_height_in_block * 4, m_encoder_context->width, m_encoder_context->height));
	auto origin_block_data = m_mb->GetOriginalLumaBlockData();
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<16, 16> InterP16x16LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

BlockData<16, 16, int32_t> InterP16x16LumaPredictor::GetDiffData() const
{
	return m_diff_data;
}

MotionInfo InterP16x16LumaPredictor::GetMotionInfo() const
{
	return m_motion_info;
}

MotionVector InterP16x16LumaPredictor::GetMVD() const
{
    return m_mvd;
}

void InterP16x16LumaPredictor::Init()
{
	auto pos = m_mb->GetPosition();
	m_x = pos.first;
	m_y = pos.second;
	m_x_in_block = pos.first / 4;
	m_y_in_block = pos.second / 4;
	m_width_in_block = 4;
	m_height_in_block = 4;
}

__codec_end