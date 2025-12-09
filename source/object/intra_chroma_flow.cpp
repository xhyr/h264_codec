#include "intra_chroma_flow.h"

#include <algorithm>

#include "intra8_chroma_predictor.h"
#include "chroma_transformer.h"
#include "chroma_quantizer.h"
#include "inverse_chroma_quantizer.h"
#include "macroblock.h"
#include "cavlc_pre_coder_chroma_8x8.h"
#include "transform_util.h"
#include "reconstruct_util.h"
#include "cavlc_cdc_coder.h"
#include "cavlc_non_cdc_coder.h"
#include "encoder_context.h"
#include "bytes_data.h"
#include "cost_util.h"

__codec_begin

IntraChromaFlow::IntraChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context), m_predictor(std::make_unique<Intra8ChromaPredictor>(mb, encoder_context))
{
}

IntraChromaFlow::~IntraChromaFlow()
{
}

void IntraChromaFlow::SetTargetPredictionType(IntraChromaPredictionType prediction_type)
{
	m_target_prediction_type = prediction_type;
}

void IntraChromaFlow::Frontend()
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

IntraChromaPredictionType IntraChromaFlow::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

BlockData<8, 8> IntraChromaFlow::GetReconstructedData(PlaneType plane_type) 
{
	return m_reconstructed_data_map[plane_type];
}

uint8_t IntraChromaFlow::GetCBP() const
{
	return m_cbp;
}

uint32_t IntraChromaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
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

int IntraChromaFlow::GetDistortion() const
{
	return m_sse_distortion;
}

void IntraChromaFlow::Predict()
{
	m_predictor->Decide(m_target_prediction_type);
}

void IntraChromaFlow::TransformAndQuantize(PlaneType plane_type)
{
	auto diff_data = m_predictor->GetDiffData(plane_type);
	ChromaTransformer transformer(diff_data);
	transformer.Transform();

	m_quantizer = std::make_unique<ChromaQuantizer>(m_encoder_context->is_slice_intra, m_encoder_context->qp, transformer.GetDCBlock(), transformer.GetBlocks());
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

void IntraChromaFlow::InverseQuantizeAndTransform(PlaneType plane_type)
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

void IntraChromaFlow::Reconstruct(PlaneType plane_type)
{
	 m_reconstructed_data_map[plane_type] = ReconstructUtil::Reconstruct(m_diff_blocks, m_predictor->GetPredictedData(plane_type));
}

void IntraChromaFlow::CalculateDistortion()
{
	m_sse_distortion = 0;
	std::vector<PlaneType> plane_types{ PlaneType::Cb, PlaneType::Cr };
	for (auto plane_type : plane_types)
	{
		auto original_block_data = m_mb->GetOriginalChromaBlockData(plane_type);
		const auto& reconstructed_block_data = m_reconstructed_data_map[plane_type];
		m_sse_distortion += CostUtil::CalculateSSEDistortion(original_block_data, reconstructed_block_data);
	}
}

__codec_end

