#include "inter_luma_flow_base.h"

#include "macroblock.h"
#include "cost_util.h"
#include "mb_util.h"
#include "block_util.h"
#include "transform_util.h"
#include "quantize_util.h"
#include "cavlc_non_cdc_coder.h"
#include "encoder_context.h"
#include "cavlc_pre_coder_luma_4x4.h"
#include "cavlc_constant_values.h"
#include "bytes_data.h"
#include "inter_p16x16_luma_flow.h"
#include "inter_p16x8_luma_flow.h"
#include "inter_p8x16_luma_flow.h"
#include "inter_p8x8_luma_flow.h"

__codec_begin

InterLumaFlowBase::InterLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

InterLumaFlowBase::~InterLumaFlowBase()
{
}

uint32_t InterLumaFlowBase::OutputSubMBTypes(std::shared_ptr<BytesData> bytes_data)
{
	return 0;
}

uint32_t InterLumaFlowBase::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	uint32_t total_bit_count = 0;

	for (uint32_t block_8x8 = 0; block_8x8 < 4; ++block_8x8)
	{
		if ((m_cbp & (1 << block_8x8)) == 0)
			continue;

		total_bit_count += OutputCoefficients(block_8x8, bytes_data);
	}

	return total_bit_count;
}

BlockData<16, 16> InterLumaFlowBase::GetReconstructedData() const
{
	return m_reconstructed_data;
}

int InterLumaFlowBase::GetDistortion() const
{
	return m_distortion;
}

uint8_t InterLumaFlowBase::GetCBP() const
{
	return m_cbp;
}

uint32_t InterLumaFlowBase::GetDetailedCBP() const
{
    return m_detailed_cbp;
}

void InterLumaFlowBase::CalculateDistortion()
{
	auto original_block_data = m_mb->GetOriginalLumaBlockData();
	m_distortion = CostUtil::CalculateSADDistortion(original_block_data, m_reconstructed_data);
}

void InterLumaFlowBase::TransformAndQuantize()
{
	m_residual_datas.resize(16);
	m_coefficient_cost = 0;
	for (uint32_t block_8x8 = 0; block_8x8 < 4; ++block_8x8)
		TransformAndQuantize(block_8x8);

	if (m_coefficient_cost <= CavlcConstantValues::s_mb_luma_coeff_cost_threshold)
	{
		m_cbp = 0;
		m_detailed_cbp = 0;
	}
}

void InterLumaFlowBase::InverseQuantizeAndTransform()
{
	for (uint32_t block_8x8 = 0; block_8x8 < 4; ++block_8x8)
		InverseQuantizeAndTransform(block_8x8);
}

void InterLumaFlowBase::TransformAndQuantize(uint32_t block_8x8)
{
	m_residual_datas.resize(16);

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

void InterLumaFlowBase::CheckCoefficientCost(uint32_t block_8x8)
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
	else
		m_cbp &= ~(1 << block_8x8);
}

void InterLumaFlowBase::InverseQuantizeAndTransform(uint32_t block_8x8)
{
	for (uint32_t y_in_block = block_8x8 / 2 * 2; y_in_block < block_8x8 / 2 * 2 + 2; ++y_in_block)
	{
		for (uint32_t x_in_block = block_8x8 % 2 * 2; x_in_block < block_8x8 % 2 * 2 + 2; ++x_in_block)
		{
			auto block_data = QuantizeUtil::InverseQuantizeNormal(m_encoder_context->qp, m_residual_datas[x_in_block + 4 * y_in_block]);
			block_data = TransformUtil::InverseDCT(block_data);
			m_diff_data.SetBlock4x4(x_in_block, y_in_block, block_data);
		}
	}
}

uint32_t InterLumaFlowBase::OutputCoefficients(uint32_t block_8x8, std::shared_ptr<BytesData> bytes_data)
{
	uint32_t total_bit_count = 0;
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
	return total_bit_count;
}

std::shared_ptr<InterLumaFlowBase> InterLumaFlowBase::CreateFlow(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context)
{
	std::shared_ptr<InterLumaFlowBase> flow;
	switch (mb_type)
	{
	case codec::MBType::PSkip:
		break;
	case codec::MBType::P16x16:
		flow.reset(new InterP16x16LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::P16x8:
		flow.reset(new InterP16x8LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::P8x16:
		flow.reset(new InterP8x16LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::P8x8:
		flow.reset(new InterP8x8LumaFlow(mb, encoder_context));
		break;
	case codec::MBType::I4:
		break;
	case codec::MBType::I16:
		break;
	default:
		break;
	}
	return flow;
}

__codec_end