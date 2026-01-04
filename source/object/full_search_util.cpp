#include "full_search_util.h"

#include "encoder_context.h"
#include "me_util.h"
#include "motion_info.h"
#include "cost_util.h"
#include "dpb.h"

#define TRACY_ENABLE
#include <tracy/Tracy.hpp>

__codec_begin

MotionInfo FullSearchUtil::FindBestMotionInfo(const SearchInfo& search_info, std::shared_ptr<EncoderContext> encoder_context, MotionVector& mvd)
{
	int64_t min_cost = std::numeric_limits<int64_t>::max();
	MotionInfo best_motion_info;
	MotionVector best_pred_mv;

	auto ref_num = encoder_context->dpb->GetRefFrameNum();
	for (uint32_t ref_id = 0; ref_id < ref_num; ++ref_id)
	{
		auto pred_mv = MEUtil::GetPredictorMV(search_info.x_in_block, search_info.y_in_block, search_info.width_in_block, search_info.height_in_block, ref_id, encoder_context->motion_info_context);
		int64_t ref_cost = MEUtil::GetRefBitCount(ref_id, ref_num) * encoder_context->lambda_motion_fp;
		for (auto mv : encoder_context->search_motion_vectors)
		{
			auto cand_mv = pred_mv + mv;
			auto cost = MEUtil::CalculateMVCost(pred_mv, cand_mv, encoder_context->lambda_motion_fp) + ref_cost;
			auto full_pixel_mv = MotionVector{ cand_mv.x / 4, cand_mv.y / 4 };
			auto ref_frame = encoder_context->dpb->GetFrame(ref_id);
			cost += CostUtil::CalculateLumaSAD(search_info.x_in_block, search_info.y_in_block, search_info.width_in_block, search_info.height_in_block, encoder_context->yuv_frame, ref_frame, full_pixel_mv);

			if (cost < min_cost)
			{
				min_cost = cost;
				best_motion_info.mv = cand_mv;
				best_motion_info.ref_id = ref_id;
				best_pred_mv = pred_mv;
			}
		}
	}

	best_motion_info.mv = MEUtil::ClipMVRange(best_motion_info.mv, encoder_context);
	mvd = best_motion_info.mv - best_pred_mv;
	return best_motion_info;
}

__codec_end
