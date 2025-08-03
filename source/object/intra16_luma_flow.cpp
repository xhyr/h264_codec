#include "intra16_luma_flow.h"

#include "macroblock.h"
#include "intra16_luma_predictor.h"
#include "intra16_luma_transformer.h"
#include "intra16_luma_quantizer.h"
#include "inverse_intra16_luma_quantizer.h"
#include "transform_util.h"
#include "reconstruct_util.h"

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
}

uint32_t Intra16LumaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{

	return 0;
}

Intra16LumaPredictionType Intra16LumaFlow::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

void Intra16LumaFlow::Predict()
{
	m_predictor = std::make_unique<Intra16LumaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
	m_cost = m_predictor->GetCost();
}

void Intra16LumaFlow::TransformAndQuantize()
{
	Intra16LumaTransformer transformer(m_predictor->GetDiffData());
	transformer.Transform();

	auto qp = m_mb->GetQP();
	m_quantizer = std::make_unique<Intra16LumaQuantizer>(qp, transformer.GetDCBlock(), transformer.GetBlocks());
	m_quantizer->Quantize();

	m_cbp = m_quantizer->IsACAllZero() ? 0 : 15;
}

void Intra16LumaFlow::InverseQuantizeAndTransform()
{
	auto dc_block = m_quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	auto qp = m_mb->GetQP();
	InverseIntra16LumaQuantizer inverse_quantizer(qp, dc_block, m_quantizer->GetACBlocks());
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