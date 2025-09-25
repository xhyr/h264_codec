#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct MotionVector;
class MotionInfoContext;

struct MEUtil
{
	static MotionVector GetPredictorMV(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, int ref_id, std::shared_ptr<MotionInfoContext> motion_info_context);

	static std::vector<MotionVector> GenerateMotionVectors(uint32_t search_range);

	static int CalculateMVCost(const MotionVector& pred, const MotionVector& cand, int lambda);

};

__codec_end