#include "cost_util.h"

#include "transform_util.h"

__codec_begin

int CostUtil::CalculateSATD(const BlockData<16, 16, int32_t>& block_data)
{
	int satd = 0;
	BlockData<4, 4, int32_t> dc_block_data;
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
	{
		for (uint32_t x_in_block = 0; x_in_block < 4; ++x_in_block)
		{
			auto block_data_4x4 = block_data.GetBlock4x4(x_in_block, y_in_block);
			block_data_4x4 = TransformUtil::Hadamard(block_data_4x4, 1);
			auto dc_value = block_data_4x4.GetElement(0, 0);
			satd += block_data_4x4.GetAbstractSum();
			satd -= abs(dc_value);

			dc_block_data.SetElement(x_in_block, y_in_block, dc_value / 4);
		}
	}

	dc_block_data = TransformUtil::Hadamard(dc_block_data, 1);
	satd += dc_block_data.GetAbstractSum();
	satd /= 2;
	return satd;
}

__codec_end

