#include "quantize_util.h"

#include <algorithm>

#include "quantize_constants.h"
#include "math_util.h"

__codec_begin

BlockData<4, 4, int32_t> QuantizeUtil::QuantizeDC(int qp, const BlockData<4, 4, int32_t>& dc_block)
{
	auto [qp_rem, qp_offset, qp_bits] = GetQuantizeParameters(qp);

	auto dc_factor = QuantizeConstants::s_quant_matrix[qp_rem][0][0];
	auto dc_data = dc_block.GetData();
	for (auto& val : dc_data)
	{
		auto positive_val = abs(val);
		positive_val = (positive_val * dc_factor + 2 * qp_offset) >> (qp_bits + 1);
		val = val >= 0 ? positive_val : -positive_val;
	}
	
	BlockData<4, 4, int32_t> output_block;
	output_block.SetData(dc_data);
    return output_block;
}

BlockData<2, 2, int32_t> QuantizeUtil::QuantizeDC(int qp, const BlockData<2, 2, int32_t>& dc_block)
{
	auto [qp_rem, qp_offset, qp_bits] = GetQuantizeParameters(qp);

	auto dc_factor = QuantizeConstants::s_quant_matrix[qp_rem][0][0];
	auto dc_data = dc_block.GetData();
	for (auto& val : dc_data)
	{
		auto positive_val = abs(val);
		positive_val = (positive_val * dc_factor + 2 * qp_offset) >> (qp_bits + 1);
		val = val >= 0 ? positive_val : -positive_val;
	}

	BlockData<2, 2, int32_t> output_block;
	output_block.SetData(dc_data);
	return output_block;
}

BlockData<4, 4, int32_t> QuantizeUtil::InverseQuantizeDC(int qp, const BlockData<4, 4, int32_t>& dc_block)
{
	auto [qp_per, qp_rem] = GetInverseQuantizeParameters(qp);
	auto dc_factor = QuantizeConstants::s_dequant_matrix[qp_rem][0][0] << qp_per;
	auto dc_data = dc_block.GetData();
	std::transform(dc_data.begin(), dc_data.end(), dc_data.begin(), [dc_factor](int val) {return MathUtil::RightShift(val * dc_factor, 2); });

	BlockData<4, 4, int32_t> output_block;
	output_block.SetData(dc_data);
	return output_block;
}

BlockData<2, 2, int32_t> QuantizeUtil::InverseQuantizeDC(int qp, const BlockData<2, 2, int32_t>& dc_block)
{
	auto [qp_per, qp_rem] = GetInverseQuantizeParameters(qp);
	auto dc_factor = QuantizeConstants::s_dequant_matrix[qp_rem][0][0] << qp_per;
	auto dc_data = dc_block.GetData();
	std::transform(dc_data.begin(), dc_data.end(), dc_data.begin(), [dc_factor](int val) {return MathUtil::RightShift(val * dc_factor, 1); });

	BlockData<2, 2, int32_t> output_block;
	output_block.SetData(dc_data);
	return output_block;
}

BlockData<4, 4, int32_t> QuantizeUtil::QuantizeAC(int qp, const BlockData<4, 4, int32_t>& ac_block)
{
	auto [qp_rem, qp_offset, qp_bits] = GetQuantizeParameters(qp);

	BlockData<4, 4, int32_t> output_block;
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			if (x + y == 0)
				continue;

			auto val = ac_block.GetElement(x, y);
			auto scale_factor = QuantizeConstants::s_quant_matrix[qp_rem][y][x];
			auto positive_val = abs(val);
			positive_val = (positive_val * scale_factor + qp_offset) >> qp_bits;
			val = val >= 0 ? positive_val : -positive_val;
			output_block.SetElement(x, y, val);
		}
	}

	return output_block;
}

BlockData<4, 4, int32_t> QuantizeUtil::InverseQuantizeAC(int qp, const BlockData<4, 4, int32_t>& ac_block)
{
	auto [qp_per, qp_rem] = GetInverseQuantizeParameters(qp);

	BlockData<4, 4, int32_t> output_block;
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			if (x + y == 0)
				continue;

			auto val = ac_block.GetElement(x, y);
			auto scale_factor = QuantizeConstants::s_dequant_matrix[qp_rem][y][x];
			val = (val * scale_factor) << qp_per;
			output_block.SetElement(x, y, val);
		}
	}

	return output_block;
}

BlockData<4, 4, int32_t> QuantizeUtil::QuantizeNormal(int qp, const BlockData<4, 4, int32_t>& block)
{
	auto [qp_rem, qp_offset, qp_bits] = GetQuantizeParameters(qp);

	BlockData<4, 4, int32_t> output_block;
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = block.GetElement(x, y);
			auto scale_factor = QuantizeConstants::s_quant_matrix[qp_rem][y][x];
			auto positive_val = abs(val);
			positive_val = (positive_val * scale_factor + qp_offset) >> qp_bits;
			val = val >= 0 ? positive_val : -positive_val;
			output_block.SetElement(x, y, val);
		}
	}

	return output_block;
}

BlockData<4, 4, int32_t> QuantizeUtil::InverseQuantizeNormal(int qp, const BlockData<4, 4, int32_t>& block)
{
	auto [qp_per, qp_rem] = GetInverseQuantizeParameters(qp);

	BlockData<4, 4, int32_t> output_block;
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = block.GetElement(x, y);
			auto scale_factor = QuantizeConstants::s_dequant_matrix[qp_rem][y][x];
			val = (val * scale_factor) << qp_per;
			output_block.SetElement(x, y, val);
		}
	}

	return output_block;
}

std::tuple<int, int, int> QuantizeUtil::GetQuantizeParameters(int qp, bool is_intra) 
{
	int qp_per = qp / 6;
	int qp_rem = qp % 6;
	int q_bits = QuantizeConstants::s_q_bits + qp_per;
	int qp_offset = (is_intra ? QuantizeConstants::s_intra_offset : QuantizeConstants::s_inter_offset) << (q_bits - QuantizeConstants::s_offset_bits);
	return std::make_tuple(qp_rem, qp_offset, q_bits);
}

std::pair<int, int> QuantizeUtil::GetInverseQuantizeParameters(int qp)
{
	int qp_per = qp / 6;
	int qp_rem = qp % 6;
	return std::make_pair(qp_per, qp_rem);
}

__codec_end