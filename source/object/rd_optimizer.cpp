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
#include "log.h"
#include "cavlc_context.h"

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
			auto old_cavlc_context = *m_encoder_context->cavlc_context;

			double rd_cost = CalculateRDCost(mb_type);
			if (rd_cost < m_rd_cost)
			{
				LOGINFO("min_rd_cost = %lf, mb_type = %d.", rd_cost, mb_type);

				m_rd_cost = rd_cost;
				m_best_chroma_prediction_type = prediction_type;
				m_mb->SetType(mb_type);
				if (mb_type == MBType::I4)
				{
					auto intra4_prediction_types = std::dynamic_pointer_cast<Intra4LumaFlow>(m_luma_flow)->GetPredictionTypes();
					m_mb->SetIntra4LumaPredictionTypes(intra4_prediction_types);
				}
			}

			*m_encoder_context->cavlc_context = old_cavlc_context;
		}
	}
}

void RDOptimizer::Backend()
{
	m_chroma_flow = std::make_shared<ChromaFlow>(m_mb, m_encoder_context);
	m_chroma_flow->SetTargetPredictionType(m_best_chroma_prediction_type);
	m_chroma_flow->Frontend();
	m_chroma_cbp = m_chroma_flow->GetCBP();

	auto mb_type = m_mb->GetType();
	RunLumaFlow(mb_type);
	OutputMB(mb_type, m_bytes_data);
	
	m_mb->SetReconstructedLumaBlockData(m_luma_flow->GetReconstructedData());
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cb), PlaneType::Cb);
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cr), PlaneType::Cr);

	LOGINFO("mb_addr = %d, total_bit = %d.", m_mb->GetAddress(), m_bytes_data->GetBitsCount());
}

double RDOptimizer::CalculateRDCost(MBType mb_type)
{
	int distortion = m_chroma_flow->GetDistortion();
	RunLumaFlow(mb_type);
	distortion += m_luma_flow->GetDistortion();
	int rate = CalculateRate(mb_type);
	double rd_cost = distortion + m_encoder_context->lambda_mode * rate;
	return rd_cost;
}

void RDOptimizer::RunLumaFlow(MBType mb_type)
{
	switch (mb_type)
	{
	case codec::MBType::I16:
	{
		m_luma_flow = std::make_shared<Intra16LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_cbp = m_luma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;
		break;
	}
	case codec::MBType::I4:
	{
		if (m_mb->GetAddress() == 7)
			int sb = 1;

		m_luma_flow = std::make_shared<Intra4LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_cbp = m_luma_flow->GetCBP();
		m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;
		break;
	}
	default:
		break;
	}
}

int RDOptimizer::CalculateRate(MBType mb_type)
{
	auto bytes_data = std::make_shared<BytesData>();
	return OutputMB(mb_type, bytes_data);
}

int RDOptimizer::OutputMB(MBType mb_type, std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();

	auto slice = m_mb->GetSlice();
	MBBinaryer mb_binaryer(slice, m_mb->GetAddress(), bytes_data);

	if (mb_type == MBType::I16)
	{
		auto intra16_prediction_type = std::dynamic_pointer_cast<Intra16LumaFlow>(m_luma_flow)->GetPredictionType();
		auto offset = MBUtil::CalculateIntra16Offset(m_cbp, intra16_prediction_type);
		mb_binaryer.OutputMBType(mb_type, offset);
	}
	else mb_binaryer.OutputMBType(mb_type);

	m_luma_flow->OutputPredictionTypes(bytes_data);

	mb_binaryer.OutputChromaPredMode(m_chroma_flow->GetPredictionType());
	mb_binaryer.OutputCBP(m_cbp);

	if (m_cbp > 0 || mb_type == MBType::I16)
		mb_binaryer.OutputQPDelta(0);

	m_luma_flow->OutputCoefficients(bytes_data);
	m_chroma_flow->OutputCoefficients(bytes_data);

	return bytes_data->GetBitsCount() - start_bit_count;
}

__codec_end