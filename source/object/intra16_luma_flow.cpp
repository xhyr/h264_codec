#include "intra16_luma_flow.h"

#include "macroblock.h"
#include "intra16_luma_predictor.h"
#include "luma16_transformer.h"
#include "luma16_quantizer.h"
#include "inverse_intra16_luma_quantizer.h"
#include "transform_util.h"
#include "reconstruct_util.h"
#include "cavlc_pre_coder_luma_16x16.h"
#include "cavlc_non_cdc_coder.h"
#include "encoder_context.h"
#include "bytes_data.h"

__codec_begin

Intra16LumaFlow::Intra16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	IntraLumaFlowBase(mb, encoder_context)
{
}

Intra16LumaFlow::~Intra16LumaFlow()
{
}

void Intra16LumaFlow::Frontend()
{
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

	CalculateDistortion();
}

uint32_t Intra16LumaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	CavlcPreCoderLuma16x16 pre_coder;
	pre_coder.Code(m_quantizer->GetDCBlock(), m_quantizer->GetACBlocks());

	auto start_bits_count = bytes_data->GetBitsCount();

	CavlcNonCdcCoder coder(m_mb->GetAddress(), m_encoder_context->cavlc_context, bytes_data);
	coder.CodeLumaDC(pre_coder.GetDCLevelAndRuns());

	if(m_cbp & 15)
		coder.CodeLumaACs(pre_coder.GetACLevelAndRuns());

	auto finish_bits_count = bytes_data->GetBitsCount();
	return finish_bits_count - start_bits_count;
}

Intra16LumaPredictionType Intra16LumaFlow::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

void Intra16LumaFlow::Predict()
{
	m_predictor = std::make_unique<Intra16LumaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
}

void Intra16LumaFlow::TransformAndQuantize()
{
	Luma16Transformer transformer(m_predictor->GetDiffData());
	transformer.Transform();

	m_quantizer = std::make_unique<Luma16Quantizer>(m_encoder_context->is_slice_intra, m_encoder_context->qp, transformer.GetDCBlock(), transformer.GetBlocks());
	m_quantizer->Quantize();

	m_cbp = m_quantizer->IsACAllZero() ? 0 : 15;
	m_detailed_cbp = m_quantizer->GetDetailedCBP();
}

void Intra16LumaFlow::InverseQuantizeAndTransform()
{
	auto dc_block = m_quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	InverseIntra16LumaQuantizer inverse_quantizer(m_encoder_context->qp, dc_block, m_quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);
	m_diff_datas = blocks;
}

void Intra16LumaFlow::Reconstruct()
{
	m_reconstructed_data = ReconstructUtil::Reconstruct(m_diff_datas, m_predictor->GetPredictedData());
}

__codec_end