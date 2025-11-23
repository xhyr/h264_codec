#pragma once

#include <cstdint>

#include "block_data.h"
#include "math_util.h"
#include "../encoder/encoder.h"

__codec_begin

struct YUVFrame;
struct MotionVector;
struct EncoderContext;

struct CostUtil
{
	static int CalculateSATD(const BlockData<4, 4, int32_t>& block_data);

	static int CalculateSATD(const BlockData<16, 16, int32_t>& block_data);

	static int CalculateSATDDistortion(const BlockData<4, 4, int32_t>& block_data);

	static int CalculateSATDDistortion(const BlockData<8, 8, int32_t>& block_data);

	static int CalculateSATDDistortion(const BlockData<16, 16, int32_t>& block_data);

	template<size_t Width, size_t Height>
	static int CalculateSSEDistortion(const BlockData<Width, Height, int32_t>& block_data)
	{
		return ScaleForAccuracy(block_data.GetAbstractSum());
	}

	static int ScaleForAccuracy(int value);

	template<size_t Width, size_t Height, typename Ty1, typename Ty2>
	static int CalculateSADDistortion(const BlockData<Width, Height, Ty1>& left_block_data, const BlockData<Width, Height, Ty2>& right_block_data)
	{
		int distortion = 0;
		for (size_t y = 0; y < Height; ++y)
			for (size_t x = 0; x < Width; ++x)
				distortion += MathUtil::Abs(static_cast<int32_t>(left_block_data.GetElement(x, y)) - static_cast<int32_t>(right_block_data.GetElement(x, y)));
		return ScaleForAccuracy(distortion);
	}

	template<size_t Width, size_t Height, typename Ty1, typename Ty2>
	static int CalculateSSEDistortion(const BlockData<Width, Height, Ty1>& left_block_data, const BlockData<Width, Height, Ty2>& right_block_data)
	{
		int distortion = 0;
		for (size_t y = 0; y < Height; ++y)
			for (size_t x = 0; x < Width; ++x)
				distortion += MathUtil::Square(static_cast<int32_t>(left_block_data.GetElement(x, y)) - static_cast<int32_t>(right_block_data.GetElement(x, y)));
		return ScaleForAccuracy(distortion);
	}

	static int CalculateLumaSAD(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, std::shared_ptr<YUVFrame> current_frame, std::shared_ptr<YUVFrame> ref_frame, const MotionVector& mv);

	static int CalculateRDCostMode(int64_t distortion, int64_t rate, std::shared_ptr<EncoderContext> encoder_context);

	static int CalculateRDCostMotion(int64_t distortion, int64_t rate, std::shared_ptr<EncoderContext> encoder_context);

};

__codec_end

