#include "me_util.h"

#include "motion_info.h"
#include "motion_info_context.h"
#include "codec_types.h"
#include "math_util.h"
#include "coding_util.h"
#include "encoder_context.h"

__codec_begin

MotionVector MEUtil::GetPredictorMV(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, int ref_id, std::shared_ptr<MotionInfoContext> motion_info_context)
{
	auto [mi_a, mi_b, mi_c] = motion_info_context->GetNeighborMotionInfo(x_in_block, y_in_block, width_in_block);

	int left_ref_id = mi_a.ref_id;
	int up_ref_id = mi_b.ref_id;
	int ur_ref_id = mi_c.ref_id;

	MvPredType mv_pred_type = MvPredType::MEDIAN;

	if (left_ref_id == ref_id && up_ref_id != ref_id && ur_ref_id != ref_id)
		mv_pred_type = MvPredType::L;
	else if (left_ref_id != ref_id && up_ref_id == ref_id && ur_ref_id != ref_id)
		mv_pred_type = MvPredType::U;
	else if (left_ref_id != ref_id && up_ref_id != ref_id && ur_ref_id == ref_id)
		mv_pred_type = MvPredType::UR;

	if (width_in_block == 2 && height_in_block == 4)
	{
		if (x_in_block % 4 == 0)
		{
			if (left_ref_id == ref_id)
				mv_pred_type = MvPredType::L;
		}
		else
		{
			if (up_ref_id == ref_id)
				mv_pred_type = MvPredType::UR;
		}
	}
	else if (width_in_block == 4 && height_in_block == 2)
	{
		if (y_in_block % 4 == 0)
		{
			if (up_ref_id == ref_id)
				mv_pred_type = MvPredType::U;
		}
		else
		{
			if (left_ref_id == ref_id)
				mv_pred_type = MvPredType::L;
		}
	}

	MotionVector mv;
	switch (mv_pred_type)
	{
	case codec::MvPredType::MEDIAN:
	{
		if (up_ref_id == -1 || ur_ref_id == -1)
		{
			if (left_ref_id != -1)
				mv = mi_a.mv;
		}
		else
		{
			MotionVector mv_a = mi_a.mv;
			MotionVector mv_b = mi_b.mv;
			MotionVector mv_c = mi_c.mv;

			mv.x = MathUtil::Median(mv_a.x, mv_b.x, mv_c.x);
			mv.y = MathUtil::Median(mv_a.y, mv_b.y, mv_c.y);
		}
	}
	break;
	case codec::MvPredType::L:
	{
		if (left_ref_id != -1)
			mv = mi_a.mv;
		break;
	}
	case codec::MvPredType::U:
	{
		if (up_ref_id != -1)
			mv = mi_b.mv;
		break;
	}
	case codec::MvPredType::UR:
	{
		if (ur_ref_id != -1)
			mv = mi_c.mv;
		break;
	}
	default:
		break;
	}

	return mv;
}

std::vector<MotionVector> MEUtil::GenerateMotionVectors(uint32_t search_range)
{
	std::vector<MotionVector> mvs;
	mvs.reserve((2 * search_range + 1) * (2 * search_range + 1));

	mvs.emplace_back(0, 0);

	for (int d = 1; d <= search_range; ++d)
	{
		for (int x = -d + 1; x < d; ++x)
		{
			mvs.emplace_back(x * 4, -d * 4);
			mvs.emplace_back(x * 4, d * 4);
		}

		for (int y = -d; y <= d; ++y)
		{
			mvs.emplace_back(-d * 4, y * 4);
			mvs.emplace_back(d * 4, y * 4);
		}
	}

	return mvs;
}

int MEUtil::CalculateMVCost(const MotionVector& pred, const MotionVector& cand, int lambda)
{
	int cost = CodingUtil::SE_V(pred.x - cand.x, nullptr) + CodingUtil::SE_V(pred.y - cand.y, nullptr);
	return cost * lambda;
}

MotionVector MEUtil::ClipMVRange(const MotionVector& mv, std::shared_ptr<EncoderContext> encoder_context)
{
	int mv_x = MathUtil::Clamp(mv.x, encoder_context->mv_horizontal_limit.first, encoder_context->mv_horizontal_limit.second);
	int mv_y = MathUtil::Clamp(mv.y, encoder_context->mv_vertical_limit.first, encoder_context->mv_vertical_limit.second);
	return { mv_x, mv_y };
}

bool MEUtil::IsMVDifferent(const MotionVector& left, const MotionVector& right, int threshold)
{
	return abs(left.x - right.x) >= threshold || abs(left.y - right.y) >= threshold;
}

__codec_end

