#pragma once

#include <cstdint>

#include "block_data.h"
#include "math_util.h"

__codec_begin

struct CostUtil
{
	static int CalculateSATD(const BlockData<16, 16, int32_t>& block_data);

	static int CalculateSATD(const BlockData<8, 8, int32_t>& block_data);

	static int CalculateSATD(const BlockData<4, 4, int32_t>& block_data);

	template<size_t Width, size_t Height>
	static int CalculateSAD(const BlockData<Width, Height>& left_block_data, const BlockData<Width, Height>& right_block_data)
	{
		int distortion = 0;
		for (size_t y = 0; y < Height; ++y)
			for (size_t x = 0; x < Width; ++x)
				distortion += MathUtil::Square(left_block_data.GetElement(x, y) - right_block_data.GetElement(x, y));
		return distortion;
	}
};

__codec_end

