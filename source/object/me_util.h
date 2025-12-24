#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct MotionVector;
class MotionInfoContext;
struct EncoderContext;
class BytesData;

struct MEUtil
{
	static MotionVector GetPredictorMV(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, int ref_id, std::shared_ptr<MotionInfoContext> motion_info_context);
	
	static MotionVector GetPredictorMVPSkip(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, int ref_id, std::shared_ptr<MotionInfoContext> motion_info_context);

	static std::vector<MotionVector> GenerateMotionVectors(uint32_t search_range);

	static int CalculateMVCost(const MotionVector& pred, const MotionVector& cand, int lambda);

	static MotionVector ClipMVRange(const MotionVector& mv, std::shared_ptr<EncoderContext> encoder_context);

	static bool IsMVDifferent(const MotionVector& left, const MotionVector& right, int threshold);

	static int GetRefBitCount(int ref_id, int ref_num);

	static uint32_t CodingRefID(int ref_id, int ref_num, std::shared_ptr<BytesData> bytes_data);


};

__codec_end