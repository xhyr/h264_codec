#include "chroma_flow.h"

#include <algorithm>

#include "intra8_chroma_predictor.h"
#include "intra8_chroma_transformer.h"
#include "intra8_chroma_quantizer.h"
#include "inverse_intra8_chroma_quantizer.h"
#include "macroblock.h"
#include "cavlc_coder_chroma_8x8.h"
#include "transform_util.h"
#include "reconstruct_util.h"

__codec_begin

ChromaFlow::ChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

ChromaFlow::~ChromaFlow()
{
}

void ChromaFlow::Frontend()
{
	Predict();
	for (auto plane_type : { PlaneType::Cb, PlaneType::Cr })
	{
		TransformAndQuantize(plane_type);
		InverseQuantizeAndTransform(plane_type);
		Reconstruct(plane_type);
	}
}

void ChromaFlow::Backend()
{

}

IntraChromaPredictionType ChromaFlow::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

BlockData<8, 8> ChromaFlow::GetReconstructedData(PlaneType plane_type) 
{
	return m_reconstructed_data_map[plane_type];
}

uint8_t ChromaFlow::GetCBP() const
{
	return m_cbp;
}

void ChromaFlow::Predict()
{
	m_predictor = std::make_unique<Intra8ChromaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
}

void ChromaFlow::TransformAndQuantize(PlaneType plane_type)
{
	auto diff_data = m_predictor->GetDiffData(plane_type);
	Intra8ChromaTransformer transformer(diff_data);
	transformer.Transform();

	auto qp = m_mb->GetQP();
	m_quantizer = std::make_unique<Intra8ChromaQuantizer>(qp, transformer.GetDCBlock(), transformer.GetBlocks());
	m_quantizer->Quantize();

	CavlcCoderChroma8x8 coder;
	coder.Code(m_quantizer->GetDCBlock(), m_quantizer->GetACBlocks());

	m_cbp = std::max(m_cbp, coder.GetCodedBlockPattern());

	if (coder.HasResetCofficients())
		m_quantizer->ResetACToZeros();
}

void ChromaFlow::InverseQuantizeAndTransform(PlaneType plane_type)
{
	auto dc_block = m_quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	auto qp = m_mb->GetQP();
	InverseIntra8ChromaQuantizer inverse_quantizer(qp, dc_block, m_quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);
	m_diff_blocks = blocks;
}

void ChromaFlow::Reconstruct(PlaneType plane_type)
{
	 m_reconstructed_data_map[plane_type] = ReconstructUtil::Reconstruct(m_diff_blocks, m_predictor->GetPredictedData(plane_type));
}

__codec_end

