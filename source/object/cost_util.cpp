#include "cost_util.h"

#include "transform_util.h"
#include "rdo_constant_values.h"

__codec_begin

int CostUtil::CalculateSATD(const BlockData<4, 4, int32_t>& block_data)
{
	auto transformed_block_data = TransformUtil::Hadamard(block_data, 0);
	auto block_satd = transformed_block_data.GetAbstractSum();
	return MathUtil::RightShift(block_satd, 1);
}

int CostUtil::CalculateSATD(const BlockData<16, 16, int32_t>& block_data)
{
	int satd = 0;
	BlockData<4, 4, int32_t> dc_block_data;
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
	{
		for (uint32_t x_in_block = 0; x_in_block < 4; ++x_in_block)
		{
			auto block_data_4x4 = block_data.GetBlock4x4(x_in_block, y_in_block);
			block_data_4x4 = TransformUtil::Hadamard(block_data_4x4);
			auto dc_value = block_data_4x4.GetElement(0, 0);
			satd += block_data_4x4.GetAbstractSum();
			satd -= abs(dc_value);

			dc_block_data.SetElement(x_in_block, y_in_block, dc_value >> 1);
		}
	}

	dc_block_data = TransformUtil::Hadamard(dc_block_data);
	satd += dc_block_data.GetAbstractSum();
	return satd;
}

int CostUtil::CalculateSATDDistortion(const BlockData<4, 4, int32_t>& block_data)
{
	auto satd = CalculateSATD(block_data);
	return ScaleForAccuracy(satd);
}

int CostUtil::CalculateSATDDistortion(const BlockData<8, 8, int32_t>& block_data)
{
	int satd = 0;
	for (uint32_t y_in_block = 0; y_in_block < 2; ++y_in_block)
	{
		for (uint32_t x_in_block = 0; x_in_block < 2; ++x_in_block)
		{
			auto block_data_4x4 = block_data.GetBlock4x4(x_in_block, y_in_block);
			satd += CalculateSATDDistortion(block_data_4x4);
		}
	}
	return satd;
}

int CostUtil::CalculateSATDDistortion(const BlockData<16, 16, int32_t>& block_data)
{
	auto satd = CalculateSATD(block_data);
	return ScaleForAccuracy(satd);
}

int CostUtil::CalculateSSEDistortion(const BlockData<4, 4, int32_t>& block_data)
{
	return ScaleForAccuracy(block_data.GetAbstractSum());
}

int CostUtil::ScaleForAccuracy(int value)
{
	return value << RDOConstantValues::s_lambda_accuracy_bits;
}

__codec_end

