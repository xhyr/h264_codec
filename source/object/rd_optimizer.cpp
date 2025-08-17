#include "rd_optimizer.h"

#include <vector>

#include "prediction_type.h"
#include "chroma_flow.h"
#include "intra16_luma_flow.h"
#include "intra4_luma_flow.h"
#include "encoder_context.h"
#include "bytes_data.h"
#include "macroblock.h"
#include "mb_binaryer.h"
#include "mb_util.h"

__codec_begin

RDOptimizer::RDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, std::shared_ptr<BytesData> bytes_data) :
	m_mb(mb), m_encoder_context(encoder_context), m_bytes_data(bytes_data)
{
}

RDOptimizer::~RDOptimizer()
{
}

void RDOptimizer::Frontend()
{
	m_rd_cost = std::numeric_limits<double>::max();

	std::vector<MBType> mb_types{ MBType::I16, MBType::I4 };
	std::vector<IntraChromaPredictionType> prediction_types{ IntraChromaPredictionType::DC, IntraChromaPredictionType::Horizontal, IntraChromaPredictionType::Vertical, IntraChromaPredictionType::Plane };
	for (auto prediction_type : prediction_types)
	{
		m_chroma_flow = std::make_shared<ChromaFlow>(m_mb, m_encoder_context);
		m_chroma_flow->SetTargetPredictionType(prediction_type);
		m_chroma_flow->Frontend();
		m_chroma_cbp = m_chroma_flow->GetCBP();

		for (auto mb_type : mb_types)
		{
			double rd_cost = CalculateRDCost(mb_type);
			if (rd_cost < m_rd_cost)
			{
				m_rd_cost = rd_cost;
				m_best_bytes_data = m_tmp_bytes_data;
				m_mb->SetType(mb_type);
				if (mb_type == MBType::I4)
				{
					auto intra4_prediction_types = std::dynamic_pointer_cast<Intra4LumaFlow>(m_luma_flow)->GetPredictionTypes();
					m_mb->SetIntra4LumaPredictionTypes(intra4_prediction_types);
				}
			}
		}
	}
}

void RDOptimizer::Backend()
{
	m_bytes_data->Push(m_best_bytes_data);
}

double RDOptimizer::CalculateRDCost(MBType mb_type)
{
	int distortion = m_chroma_flow->GetDistortion();
	switch (mb_type)
	{
	case codec::MBType::I16:
	{
		m_luma_flow = std::make_shared<Intra16LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		distortion += m_luma_flow->GetDistortion();
		m_luma_cbp = m_luma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;
		break;
	}
	case codec::MBType::I4:
	{
		m_luma_flow = std::make_shared<Intra4LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		distortion += m_luma_flow->GetDistortion();
		m_luma_cbp = m_luma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;
		break;
	}
	default:
		break;
	}

	int rate = CalculateRate(mb_type);
	double rd_cost = distortion + m_encoder_context->lambda_mode * rate;
	return rd_cost;
}

int RDOptimizer::CalculateRate(MBType mb_type)
{
	auto slice = m_mb->GetSlice();
	m_tmp_bytes_data = std::make_shared<BytesData>();
	MBBinaryer mb_binaryer(slice, m_mb->GetAddress(), m_tmp_bytes_data);

	if (mb_type == MBType::I16)
	{
		auto intra16_prediction_type = std::dynamic_pointer_cast<Intra16LumaFlow>(m_luma_flow)->GetPredictionType();
		auto offset = MBUtil::CalculateIntra16Offset(m_cbp, intra16_prediction_type);
		mb_binaryer.OutputMBType(mb_type, offset);
	}
	else mb_binaryer.OutputMBType(mb_type);

	m_luma_flow->OutputPredictionTypes(m_tmp_bytes_data);

	mb_binaryer.OutputChromaPredMode(m_chroma_flow->GetPredictionType());
	mb_binaryer.OutputCBP(m_cbp);

	if (m_cbp > 0 || mb_type == MBType::I16)
		mb_binaryer.OutputQPDelta(0);

	m_luma_flow->OutputCoefficients(m_tmp_bytes_data);
	m_chroma_flow->OutputCoefficients(m_tmp_bytes_data);

	return m_tmp_bytes_data->GetBitsCount();
}

__codec_end