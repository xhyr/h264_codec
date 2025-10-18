#include "inter_p8x16_chroma_flow.h"

#include "inter_p8x16_chroma_predictor.h"
#include "bytes_data.h"
#include "encoder_context.h"
#include "cavlc_cdc_coder.h"
#include "cavlc_non_cdc_coder.h"
#include "chroma_transformer.h"
#include "chroma_quantizer.h"
#include "inverse_chroma_quantizer.h"
#include "cavlc_pre_coder_chroma_8x8.h"
#include "transform_util.h"
#include "reconstruct_util.h"
#include "macroblock.h"
#include "cost_util.h"

__codec_begin

InterP8x16ChromaFlow::InterP8x16ChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterChromaFlowBase(mb, encoder_context)
{
	for(uint8_t i = 0; i < 2; ++i)
		m_predictors[i] = std::make_unique<InterP8x16ChromaPredictor>(mb, encoder_context, i);
}

InterP8x16ChromaFlow::~InterP8x16ChromaFlow()
{
}

void InterP8x16ChromaFlow::Frontend()
{
	Predict();
	for (auto plane_type : { PlaneType::Cb, PlaneType::Cr })
	{
		TransformAndQuantize(plane_type);
		InverseQuantizeAndTransform(plane_type);
		Reconstruct(plane_type);
	}
	CalculateDistortion();
}

uint32_t InterP8x16ChromaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bits_count = bytes_data->GetBitsCount();

	if (m_cbp > 0)
	{
		CavlcCdcCoder coder(m_encoder_context->mb_addr, m_encoder_context->cavlc_context, bytes_data);
		coder.CodeChromaDC(CavlcDataType::CbDC, m_cavlc_data_source.cb_dc);
		coder.CodeChromaDC(CavlcDataType::CrDC, m_cavlc_data_source.cr_dc);
	}

	if (m_cbp > 1)
	{
		CavlcNonCdcCoder coder(m_encoder_context->mb_addr, m_encoder_context->cavlc_context, bytes_data);
		coder.CodeChromaACs(CavlcDataType::CbAC, m_cavlc_data_source.cb_acs);
		coder.CodeChromaACs(CavlcDataType::CrAC, m_cavlc_data_source.cr_acs);
	}


	auto finish_bits_count = bytes_data->GetBitsCount();
	return finish_bits_count - start_bits_count;
}

void InterP8x16ChromaFlow::Predict()
{
	for (uint8_t i = 0; i < 2; ++i)
		m_predictors[i]->Decide();
}

void InterP8x16ChromaFlow::TransformAndQuantize(PlaneType plane_type)
{
	auto diff_data = GetDiffData(plane_type);
	ChromaTransformer transformer(diff_data);
	transformer.Transform();

	m_quantizer = std::make_unique<ChromaQuantizer>(m_encoder_context->qp, transformer.GetDCBlock(), transformer.GetBlocks(), false);
	m_quantizer->Quantize();

	CavlcPreCoderChroma8x8 pre_coder;
	pre_coder.Code(m_quantizer->GetDCBlock(), m_quantizer->GetACBlocks());

	m_cbp = std::max(m_cbp, pre_coder.GetCodedBlockPattern());

	if (pre_coder.HasResetCofficients())
		m_quantizer->ResetACToZeros();

	if (plane_type == PlaneType::Cb)
	{
		m_cavlc_data_source.cb_dc = pre_coder.GetDCLevelAndRuns();
		m_cavlc_data_source.cb_acs = pre_coder.GetACLevelAndRuns();
	}
	else
	{
		m_cavlc_data_source.cr_dc = pre_coder.GetDCLevelAndRuns();
		m_cavlc_data_source.cr_acs = pre_coder.GetACLevelAndRuns();
	}
}

void InterP8x16ChromaFlow::InverseQuantizeAndTransform(PlaneType plane_type)
{
	auto dc_block = m_quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	InverseChromaQuantizer inverse_quantizer(m_encoder_context->qp, dc_block, m_quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);
	m_diff_blocks = blocks;
}

void InterP8x16ChromaFlow::Reconstruct(PlaneType plane_type)
{
	m_reconstructed_data_map[plane_type] = ReconstructUtil::Reconstruct(m_diff_blocks, GetPredictedData(plane_type));
}

void InterP8x16ChromaFlow::CalculateDistortion()
{
	m_distortion = 0;
	std::vector<PlaneType> plane_types{ PlaneType::Cb, PlaneType::Cr };
	for (auto plane_type : plane_types)
	{
		auto original_block_data = m_mb->GetOriginalChromaBlockData(plane_type);
		const auto& reconstructed_block_data = m_reconstructed_data_map[plane_type];
		m_distortion += CostUtil::CalculateSADDistortion(original_block_data, reconstructed_block_data);
	}
}

BlockData<8, 8, int32_t> InterP8x16ChromaFlow::GetDiffData(PlaneType plane_type) const
{
	BlockData<8, 8, int32_t> ret;
	for (uint8_t i = 0; i < 2; ++i)
	{
		auto diff_data_blocks = m_predictors[i]->GetDiffData(plane_type).GetTotalBlock4x4s();
		ret.SetBlock4x4(i, 0, diff_data_blocks[0]);
		ret.SetBlock4x4(i, 1, diff_data_blocks[1]);
	}
	return ret;
}

BlockData<8, 8> InterP8x16ChromaFlow::GetPredictedData(PlaneType plane_type) const
{
	BlockData<8, 8> ret;
	for (uint8_t i = 0; i < 2; ++i)
	{
		auto predicted_data_blocks = m_predictors[i]->GetPredictedData(plane_type).GetTotalBlock4x4s<uint8_t>();
		ret.SetBlock4x4(i, 0, predicted_data_blocks[0]);
		ret.SetBlock4x4(i, 1, predicted_data_blocks[1]);
	}
	return ret;
}

__codec_end
