#include "intra16_luma_reconstructor.h"

#include "math_util.h"
#include "quantize_constants.h"
#include "common_constant_values.h"

__codec_begin

Intra16LumaReconstructor::Intra16LumaReconstructor(const std::vector<BlockData<4, 4, int32_t>>& residual_blocks, const BlockData<16, 16>& predicted_data) :
	m_residual_blocks(residual_blocks), m_predicted_data(predicted_data)
{
}

Intra16LumaReconstructor::~Intra16LumaReconstructor()
{
}

void Intra16LumaReconstructor::Reconstruct()
{
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			auto predicted_val = m_predicted_data.GetElement(x, y);
			uint32_t block_index = (y / 4) * 4 + x / 4;
			auto residual_val = m_residual_blocks[block_index].GetElement(x % 4, y % 4);
			auto val = MathUtil::RightShift(residual_val + (predicted_val << QuantizeConstants::s_dq_bits), QuantizeConstants::s_dq_bits);
			val = MathUtil::Clamp<int>(val, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			m_block_data.SetElement(x, y, val);
		}
	}
}

BlockData<16, 16> Intra16LumaReconstructor::GetBlockData() const
{
	return m_block_data;
}

__codec_end


