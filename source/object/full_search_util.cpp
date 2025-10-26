#include "full_search_util.h"

#include "encoder_context.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"

__codec_begin

MotionVector FullSearchUtil::FindBestMV(const SearchInfo& search_info, std::shared_ptr<EncoderContext> encoder_context, MotionVector& mvd)
{
	auto pred_mv = MEUtil::GetPredictorMV(search_info.x_in_block, search_info.y_in_block, search_info.width_in_block, search_info.height_in_block, 0, encoder_context->motion_info_context);

	int min_cost = std::numeric_limits<int>::max();
	MotionVector best_mv;
	for (auto mv : encoder_context->search_motion_vectors)
	{
		auto cand_mv = pred_mv + mv;
		auto cost = MEUtil::CalculateMVCost(pred_mv, cand_mv, encoder_context->lambda_motion_fp);
		auto full_pixel_mv = MotionVector{ cand_mv.x / 4, cand_mv.y / 4 };
		cost += CostUtil::CalculateLumaSAD(search_info.x_in_block, search_info.y_in_block, search_info.width_in_block, search_info.height_in_block, encoder_context->yuv_frame, encoder_context->last_frame, full_pixel_mv);

		if (cost < min_cost)
		{
			min_cost = cost;
			best_mv = cand_mv;
		}
	}

	best_mv = MEUtil::ClipMVRange(best_mv, encoder_context);
	mvd = best_mv - pred_mv;
	return best_mv;
}

__codec_end
