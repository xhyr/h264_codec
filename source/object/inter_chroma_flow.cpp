#include "inter_chroma_flow.h"

#include "inter_p4x4_chroma_predictor.h"
#include "encoder_context.h"
#include "cavlc_cdc_coder.h"
#include "cavlc_non_cdc_coder.h"
#include "bytes_data.h"
#include "chroma_transformer.h"
#include "chroma_quantizer.h"
#include "cavlc_pre_coder_chroma_8x8.h"
#include "transform_util.h"
#include "inverse_chroma_quantizer.h"
#include "reconstruct_util.h"
#include "macroblock.h"
#include "cost_util.h"


__codec_begin

InterChromaFlow::InterChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
	for (uint8_t i = 0; i < 4; ++i)
		m_predictors[i] = std::make_unique<InterP4x4ChromaPredictor>(mb, encoder_context, i);
}

InterChromaFlow::~InterChromaFlow()
{
}

void InterChromaFlow::Frontend()
{
	for (auto plane_type : { PlaneType::Cb, PlaneType::Cr })
	{
		Predict();
		TransformAndQuantize(plane_type);
		InverseQuantizeAndTransform(plane_type);
		Reconstruct(plane_type);
	}
	CalculateDistortion();
}

uint32_t InterChromaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
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

BlockData<8, 8> InterChromaFlow::GetReconstructedData(PlaneType plane_type)
{
	return m_reconstructed_data_map[plane_type];
}

uint8_t InterChromaFlow::GetCBP() const
{
	return m_cbp;
}

int InterChromaFlow::GetDistortion() const
{
	return m_distortion;
}

void InterChromaFlow::Predict()
{
	for (uint8_t i = 0; i < 4; ++i)
	{
		for (auto plane_type : { PlaneType::Cb, PlaneType::Cr })
		{
			m_predictors[i]->Decide();
			m_predictors[i]->FillDiffData(plane_type, m_diff_datas_map[plane_type]);
			m_predictors[i]->FillPredictedData(plane_type, m_predicted_data_map[plane_type]);
		}
	}
}

void InterChromaFlow::TransformAndQuantize(PlaneType plane_type)
{
	ChromaTransformer transformer(m_diff_datas_map[plane_type]);
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

void InterChromaFlow::InverseQuantizeAndTransform(PlaneType plane_type)
{
	auto dc_block = m_quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	InverseChromaQuantizer inverse_quantizer(m_encoder_context->qp, dc_block, m_quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);
	m_diff_datas_map[plane_type] = blocks;
}

void InterChromaFlow::Reconstruct(PlaneType plane_type)
{
	m_reconstructed_data_map[plane_type] = ReconstructUtil::Reconstruct(m_diff_datas_map[plane_type], m_predicted_data_map[plane_type]);
}

void InterChromaFlow::CalculateDistortion()
{
	m_distortion = 0;
	for (auto plane_type : { PlaneType::Cb, PlaneType::Cr })
	{
		auto original_block_data = m_mb->GetOriginalChromaBlockData(plane_type);
		const auto& reconstructed_block_data = m_reconstructed_data_map[plane_type];
		m_distortion += CostUtil::CalculateSADDistortion(original_block_data, reconstructed_block_data);
	}
}

__codec_end