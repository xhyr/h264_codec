#include "filter_util.h"

#include "macroblock.h"
#include "mb_util.h"
#include "motion_info.h"
#include "me_util.h"
#include "me_constant_values.h"

__codec_begin

std::vector<uint8_t> FilterUtil::GetVerticalStrengths(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index)
{
	std::vector<uint8_t> strengths;
	strengths.reserve(4);

	if (!MBUtil::IsIntra(mb_q))
	{
		if (!MBUtil::IsIntra(mb_p))
		{
			for (uint8_t i = 0; i < 4; ++i)
			{
				if (!IsLumaAllZeroVerticalEdge(mb_p, mb_q, edge_index, i))
					strengths.push_back(2);
				else if (edge_index > 0 && (mb_q->GetType() == MBType::P16x16 || mb_q->GetType() == MBType::P16x8))
					strengths.push_back(0);
				else if (IsMotionDifferentVerticalEdge(mb_p, mb_q, edge_index, i))
					strengths.push_back(1);
				else
					strengths.push_back(0);
			}

		}
		else
		{
			uint8_t strength = edge_index == 0 ? 4 : 3;
			strengths.resize(4, strength);
		}
	}
	else
	{
		uint8_t strength = edge_index == 0 ? 4 : 3;
		strengths.resize(4, strength);
	}

	return strengths;
}

std::vector<uint8_t> FilterUtil::GetHorizontalStrengths(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index)
{
	std::vector<uint8_t> strengths;
	strengths.reserve(4);

	if (!MBUtil::IsIntra(mb_q))
	{
		if (!MBUtil::IsIntra(mb_p))
		{
			for (uint8_t i = 0; i < 4; ++i)
			{
				if (!IsLumaAllZeroHorizontalEdge(mb_p, mb_q, edge_index, i))
					strengths.push_back(2);
				else if (edge_index > 0 && (mb_q->GetType() == MBType::P16x16 || mb_q->GetType() == MBType::P8x16))
					strengths.push_back(0);
				else if (IsMotionDifferentHorizontalEdge(mb_p, mb_q, edge_index, i))
					strengths.push_back(1);
				else
					strengths.push_back(0);
			}
		}
		else
		{
			uint8_t strength = edge_index == 0 ? 4 : 3;
			strengths.resize(4, strength);
		}
	}
	else
	{
		uint8_t strength = edge_index == 0 ? 4 : 3;
		strengths.resize(4, strength);
	}

	return strengths;
}

bool FilterUtil::IsLumaAllZeroVerticalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i)
{
	uint32_t p_cbp = mb_p->GetLumaDetailedCBP();
	uint32_t q_cbp = mb_q->GetLumaDetailedCBP();

	if (edge_index == 0)
		return MBUtil::IsLumaAllZero(p_cbp, 3, i) && MBUtil::IsLumaAllZero(q_cbp, 0, i);
	else
		return MBUtil::IsLumaAllZero(q_cbp, edge_index - 1, i) && MBUtil::IsLumaAllZero(q_cbp, edge_index, i);
}

bool FilterUtil::IsLumaAllZeroHorizontalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i)
{
	uint32_t p_cbp = mb_p->GetLumaDetailedCBP();
	uint32_t q_cbp = mb_q->GetLumaDetailedCBP();

	if (edge_index == 0)
		return MBUtil::IsLumaAllZero(p_cbp, i, 3) && MBUtil::IsLumaAllZero(q_cbp, i, 0);
	else
		return MBUtil::IsLumaAllZero(q_cbp,i, edge_index - 1) && MBUtil::IsLumaAllZero(q_cbp, i, edge_index);
}

bool FilterUtil::IsMotionDifferentVerticalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i)
{
	if (edge_index == 0)
	{
		auto left_motion_info = mb_p->GetMotionInfo(3, i);
		auto right_motion_info = mb_q->GetMotionInfo(0, i);
		return left_motion_info.ref_id != right_motion_info.ref_id || MEUtil::IsMVDifferent(left_motion_info.mv, right_motion_info.mv, MEConstantValues::mv_different_threshold);
	}
	else
	{
		auto left_motion_info = mb_q->GetMotionInfo(edge_index - 1, i);
		auto right_motion_info = mb_q->GetMotionInfo(edge_index, i);
		return left_motion_info.ref_id != right_motion_info.ref_id || MEUtil::IsMVDifferent(left_motion_info.mv, right_motion_info.mv, MEConstantValues::mv_different_threshold);
	}
}

bool FilterUtil::IsMotionDifferentHorizontalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i)
{
	if (edge_index == 0)
	{
		auto up_motion_info = mb_p->GetMotionInfo(i, 3);
		auto down_motion_info = mb_q->GetMotionInfo(i, 0);
		return up_motion_info.ref_id != down_motion_info.ref_id || MEUtil::IsMVDifferent(up_motion_info.mv, down_motion_info.mv, MEConstantValues::mv_different_threshold);
	}
	else
	{
		auto up_motion_info = mb_q->GetMotionInfo(i, edge_index - 1);
		auto down_motion_info = mb_q->GetMotionInfo(i, edge_index);
		return down_motion_info.ref_id != up_motion_info.ref_id || MEUtil::IsMVDifferent(down_motion_info.mv, up_motion_info.mv, MEConstantValues::mv_different_threshold);
	}
}

__codec_end

