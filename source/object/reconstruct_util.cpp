#include "reconstruct_util.h"

#include "math_util.h"
#include "quantize_constants.h"
#include "common_constant_values.h"

__codec_begin

uint8_t ReconstructUtil::Reconstruct(uint8_t predicted_val, int32_t residual_val)
{
	auto val = MathUtil::RightShift(residual_val + (predicted_val << QuantizeConstants::s_dq_bits), QuantizeConstants::s_dq_bits);
	val = MathUtil::Clamp<int>(val, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
	return val;
}

BlockData<16, 16> ReconstructUtil::Reconstruct(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<16, 16>& predicted_data)
{
	BlockData<16, 16> reconstructed_data;
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			uint32_t block_index = (y / 4) * 4 + x / 4;
			auto residual_val = residual_blocks[block_index].GetElement(x % 4, y % 4);
			auto val = Reconstruct(predicted_val, residual_val);
			reconstructed_data.SetElement(x, y, val);
		}
	}
	return reconstructed_data;
}

BlockData<8, 8> ReconstructUtil::Reconstruct(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<8, 8>& predicted_data)
{
	BlockData<8, 8> reconstructed_data;
	for (uint32_t y = 0; y < 8; ++y)
	{
		for (uint32_t x = 0; x < 8; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			uint32_t block_index = (y / 4) * 2 + x / 4;
			auto residual_val = residual_blocks[block_index].GetElement(x % 4, y % 4);
			auto val = Reconstruct(predicted_val, residual_val);
			reconstructed_data.SetElement(x, y, val);
		}
	}
	return reconstructed_data;
}

__codec_end
