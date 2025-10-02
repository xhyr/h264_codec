#include "inter_p16x16_luma_flow.h"

#include "coding_util.h"
#include "bytes_data.h"
#include "inter_p16x16_luma_predictor.h"
#include "transform_util.h"
#include "quantize_util.h"
#include "encoder_context.h"
#include "motion_info_context.h"
#include "reconstruct_util.h"
#include "cavlc_constant_values.h"
#include "cavlc_pre_coder_luma_4x4.h"
#include "cavlc_non_cdc_coder.h"
#include "macroblock.h"
#include "block_util.h"
#include "mb_util.h"

__codec_begin

InterP16x16LumaFlow::InterP16x16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{
}

InterP16x16LumaFlow::~InterP16x16LumaFlow()
{
}

void InterP16x16LumaFlow::Frontend()
{
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

	CalculateDistortion();
}

void InterP16x16LumaFlow::Backend()
{
	auto motion_info = m_predictor->GetMotionInfo();
	m_encoder_context->motion_info_context->SetMotionInfo(m_mb->GetAddress(), 0, 0, 4, 4, motion_info);
}

uint32_t InterP16x16LumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();
	
	auto mvd = m_predictor->GetMVD();

	CodingUtil::SE_V(mvd.x, bytes_data);
	CodingUtil::SE_V(mvd.y, bytes_data);

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

uint32_t InterP16x16LumaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	if (m_mb->GetAddress() == 11)
		int sb = 1;

	uint32_t total_bit_count = 0;

	for (uint32_t block_8x8 = 0; block_8x8 < 4; ++block_8x8)
	{
		if ((m_cbp & (1 << block_8x8)) == 0)
			continue;

		for (uint32_t block_4x4 = 0; block_4x4 < 4; ++block_4x4)
		{
			uint32_t block_index = BlockUtil::CalculateBlockIndex(block_8x8, block_4x4);
			CavlcPreCoderLuma4x4 pre_coder;
			pre_coder.Code(m_residual_datas[block_index]);
			auto level_and_runs = pre_coder.GetLevelAndRuns();

			auto start_bits_count = bytes_data->GetBitsCount();

			CavlcNonCdcCoder coder(m_mb->GetAddress(), m_encoder_context->cavlc_context, bytes_data);
			coder.CodeNormalLuma(block_index, level_and_runs);

			auto finish_bits_count = bytes_data->GetBitsCount();
			auto block_used_bit_count = finish_bits_count - start_bits_count;
			total_bit_count += block_used_bit_count;
		}
	}

	return total_bit_count;
}

void InterP16x16LumaFlow::Predict()
{
	m_predictor = std::make_unique<InterP16x16LumaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
	auto diff_data = m_predictor->GetDiffData();
	m_diff_datas = diff_data.GetTotalBlock4x4s();
}

void InterP16x16LumaFlow::TransformAndQuantize()
{
	m_residual_datas.resize(16);
	for (uint32_t block_8x8 = 0; block_8x8 < 4; ++block_8x8)
	{
		bool all_zero = true;

		for (uint32_t block_4x4 = 0; block_4x4 < 4; ++block_4x4)
		{
			uint32_t block_index = BlockUtil::CalculateBlockIndex(block_8x8, block_4x4);
			auto& block_data = m_diff_datas[block_index];
			block_data = TransformUtil::DCT(block_data);
			block_data = QuantizeUtil::QuantizeNormal(m_encoder_context->qp, block_data, false);
			m_residual_datas[block_index] = block_data;
		}

		CheckCoefficientCost(block_8x8);
	}

	if (m_coefficient_cost <= CavlcConstantValues::s_mb_luma_coeff_cost_threshold)
	{
		m_cbp = 0;
		m_detailed_cbp = 0;
	}
}

void InterP16x16LumaFlow::InverseQuantizeAndTransform()
{
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
	{
		for (uint32_t x_in_block = 0; x_in_block < 4; ++x_in_block)
		{
			auto block_data = QuantizeUtil::InverseQuantizeNormal(m_encoder_context->qp, m_residual_datas[x_in_block + 4 * y_in_block]);
			block_data = TransformUtil::InverseDCT(block_data);
			m_diff_data.SetBlock4x4(x_in_block, y_in_block, block_data);
		}
	}
}

void InterP16x16LumaFlow::Reconstruct()
{
	auto predicted_data = m_predictor->GetPredictedData();
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			auto residual_val = m_cbp == 0 ? 0 : m_diff_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
			m_reconstructed_data.SetElement(x, y, val);
		}
	}
}

void InterP16x16LumaFlow::CheckCoefficientCost(uint32_t block_8x8)
{
	bool all_zero{ true };
	uint32_t coefficent_cost{ 0 };
	for (uint32_t block_4x4 = 0; block_4x4 < 4; ++block_4x4)
	{
		uint32_t block_index = BlockUtil::CalculateBlockIndex(block_8x8, block_4x4);

		CavlcPreCoderLuma4x4 pre_coder;
		pre_coder.Code(m_residual_datas[block_index]);
		coefficent_cost += pre_coder.GetCoefficientCost();
		bool block_all_zero = pre_coder.IsAllZero();
		all_zero &= block_all_zero;
		MBUtil::FillDetailedCBP(block_all_zero, block_index, m_detailed_cbp);
	}

	if (coefficent_cost <= CavlcConstantValues::s_luma_coeff_cost_threshold)
	{
		for (uint32_t block_4x4 = 0; block_4x4 < 4; ++block_4x4)
		{
			uint32_t block_index = BlockUtil::CalculateBlockIndex(block_8x8, block_4x4);
			m_residual_datas[block_index].Reset(0);
		}
		all_zero = true;
		MBUtil::ResetDetailedCBP(block_8x8, m_detailed_cbp);
	}
	else m_coefficient_cost += coefficent_cost;

	if (!all_zero)
		m_cbp |= (1 << block_8x8);
}

__codec_end