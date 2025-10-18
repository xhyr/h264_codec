#include "inter_p8x16_luma_flow.h"

#include "coding_util.h"
#include "bytes_data.h"
#include "inter_p8x16_luma_predictor.h"
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

InterP8x16LumaFlow::InterP8x16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{
}

InterP8x16LumaFlow::~InterP8x16LumaFlow()
{
}

void InterP8x16LumaFlow::Frontend()
{
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

	CalculateDistortion();
}

void InterP8x16LumaFlow::Backend()
{
	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		auto motion_info = m_predictors[segment_index]->GetMotionInfo();
		m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), 2 * segment_index, 0, 2, 4, motion_info);
	}
}

uint32_t InterP8x16LumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();

	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		auto mvd = m_predictors[segment_index]->GetMVD();

		CodingUtil::SE_V(mvd.x, bytes_data);
		CodingUtil::SE_V(mvd.y, bytes_data);
	}

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

uint32_t InterP8x16LumaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
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

void InterP8x16LumaFlow::Predict()
{
	m_diff_datas.resize(16);
	auto old_motion_infos = m_encoder_context->motion_info_context->GetMotionInfos(m_mb->GetAddress(), 0, 0, 2, 4);
	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		m_predictors[segment_index] = std::make_unique<InterP8x16LumaPredictor>(m_mb, m_encoder_context, segment_index);
		m_predictors[segment_index]->Decide();
		auto diff_data = m_predictors[segment_index]->GetDiffData();
		auto diff_block_datas = diff_data.GetTotalBlock4x4s();
	
		for (uint32_t i = 0; i < 8; ++i)
		{
			uint32_t index = (i / 2) * 4 + (i % 2) + segment_index * 2;
			m_diff_datas[index] = diff_block_datas[i];
		}

		if (segment_index == 0)
		{
			auto motion_info = m_predictors[segment_index]->GetMotionInfo();
			m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), 0, 0, 2, 4, motion_info);
		}
	}
	m_encoder_context->motion_info_context->SetMotionInfos(m_mb->GetAddress(), 0, 0, 2, 4, old_motion_infos);
}

void InterP8x16LumaFlow::TransformAndQuantize()
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

void InterP8x16LumaFlow::InverseQuantizeAndTransform()
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

void InterP8x16LumaFlow::Reconstruct()
{
	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		auto predicted_data = m_predictors[segment_index]->GetPredictedData();
		for (uint32_t y = 0; y < 16; ++y)
		{
			for (uint32_t x = segment_index * 8; x < segment_index * 8 + 8; ++x)
			{
				auto predicted_val = predicted_data.GetElement(x - segment_index * 8, y);
				auto residual_val = m_cbp == 0 ? 0 : m_diff_data.GetElement(x, y);
				auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
				m_reconstructed_data.SetElement(x, y, val);
			}
		}
	}
}

void InterP8x16LumaFlow::CheckCoefficientCost(uint32_t block_8x8)
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