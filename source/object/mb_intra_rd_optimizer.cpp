#include "mb_intra_rd_optimizer.h"

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
#include "rdo_util.h"

__codec_begin

MBIntraRDOptimizer::MBIntraRDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

MBIntraRDOptimizer::~MBIntraRDOptimizer()
{
}

void MBIntraRDOptimizer::Encode()
{
	m_rd_cost = std::numeric_limits<double>::max();

	m_mb_addr = m_mb->GetAddress();

	m_chroma_flow = std::make_shared<ChromaFlow>(m_mb, m_encoder_context);
	m_chroma_flow->Frontend();
	m_chroma_cbp = m_chroma_flow->GetCBP();

	std::vector<MBType> mb_types{ MBType::I16, MBType::I4 };
	for (auto mb_type : mb_types)
	{
		double rd_cost = CalculateRDCost(mb_type);
		auto old_mb_luma_coeff_nums = m_encoder_context->cavlc_context->GetMBLumaCoeffNums(m_mb_addr);
		if (rd_cost < m_rd_cost)
		{
			m_rd_cost = rd_cost;
			m_mb->SetType(mb_type);
			if (mb_type == MBType::I4)
			{
				auto intra4_prediction_types = std::dynamic_pointer_cast<Intra4LumaFlow>(m_luma_flow)->GetPredictionTypes();
				m_mb->SetIntra4LumaPredictionTypes(intra4_prediction_types);
			}

			m_best_luma_flow = m_luma_flow;
		}
		else m_encoder_context->cavlc_context->SetMBLumaCoeffNums(m_mb_addr, old_mb_luma_coeff_nums);
	}

	m_luma_flow = m_best_luma_flow;
	m_luma_cbp = m_luma_flow->GetCBP();
	m_cbp = (m_chroma_cbp << 4) | m_luma_cbp;

	m_mb->SetReconstructedLumaBlockData(m_luma_flow->GetReconstructedData());
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cb), PlaneType::Cb);
	m_mb->SetReconstructedChromaBlockData(m_chroma_flow->GetReconstructedData(PlaneType::Cr), PlaneType::Cr);
}

void MBIntraRDOptimizer::Binary(std::shared_ptr<BytesData> bytes_data)
{
	if (m_encoder_context->slice_addr == 7 && m_mb_addr == 17)
		int sb = 1;

	auto start_bit_count = bytes_data->GetBitsCount();
	auto mb_type = m_mb->GetType();
	OutputMB(mb_type, bytes_data);
	//LOGINFO("mb_addr = %d, total_bit = %d.", m_mb->GetAddress(), bytes_data->GetBitsCount() - start_bit_count);
}

double MBIntraRDOptimizer::CalculateRDCost(MBType mb_type)
{
	int distortion = m_chroma_flow->GetDistortion();
	RunLumaFlow(mb_type);
	distortion += m_luma_flow->GetDistortion();

	if (distortion > m_rd_cost)
		return std::numeric_limits<double>::max();

	int rate = CalculateRate(mb_type);
	double rd_cost = RDOUtil::CalculateRDCost(distortion, rate, m_encoder_context->lambda_mode_fp);
	return rd_cost;
}

void MBIntraRDOptimizer::RunLumaFlow(MBType mb_type)
{
	switch (mb_type)
	{
	case codec::MBType::I16:
	{
		m_luma_flow = std::make_shared<Intra16LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_cbp = m_luma_flow->GetCBP();
		break;
	}
	case codec::MBType::I4:
	{
		m_luma_flow = std::make_shared<Intra4LumaFlow>(m_mb, m_encoder_context);
		m_luma_flow->Frontend();
		m_luma_cbp = m_luma_flow->GetCBP();
		break;
	}
	default:
		break;
	}
	m_cbp = m_chroma_cbp << 4 | m_luma_cbp;
}

int MBIntraRDOptimizer::CalculateRate(MBType mb_type)
{
	auto bytes_data = std::make_shared<BytesData>();
	return OutputMB(mb_type, bytes_data);
}

int MBIntraRDOptimizer::OutputMB(MBType mb_type, std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();

	auto slice = m_mb->GetSlice();
	MBBinaryer mb_binaryer(slice, m_mb->GetAddress(), bytes_data);

	if (mb_type == MBType::I16)
	{
		if (m_encoder_context->slice_addr == 7 && m_mb_addr == 17)
			int sb = 1;

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