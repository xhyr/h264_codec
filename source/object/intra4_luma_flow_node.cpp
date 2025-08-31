#include "intra4_luma_flow_node.h"

#include "intra4_luma_predictor.h"
#include "transform_util.h"
#include "macroblock.h"
#include "quantize_util.h"
#include "math_util.h"
#include "reconstruct_util.h"
#include "cavlc_pre_coder_luma_4x4.h"
#include "cavlc_non_cdc_coder.h"
#include "encoder_context.h"
#include "bytes_data.h"
#include "cost_util.h"
#include "log.h"

__codec_begin

Intra4LumaFlowNode::Intra4LumaFlowNode(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, BlockData<16, 16>& reconstructed_data, std::vector<Intra4LumaPredictionType>& prediction_types, uint8_t index_in_block8x8, uint8_t index_in_block4x4) :
	m_mb(mb), m_encoder_context(encoder_context), m_reconstructed_data(reconstructed_data), m_prediction_types(prediction_types),
	m_x_in_block((index_in_block8x8 % 2) * 2 + (index_in_block4x4 % 2)),
	m_y_in_block((index_in_block8x8 / 2) * 2 + (index_in_block4x4 / 2))
{
	Init();
}

Intra4LumaFlowNode::~Intra4LumaFlowNode()
{
}

void Intra4LumaFlowNode::SetTargetPredictionType(Intra4LumaPredictionType prediction_type)
{
	m_target_prediction_type = prediction_type;
}

void Intra4LumaFlowNode::Frontend()
{
	std::vector<Intra4LumaPredictionType> prediction_types = {
		Intra4LumaPredictionType::Vertical,
		Intra4LumaPredictionType::Horizontal,
		Intra4LumaPredictionType::DC,
		Intra4LumaPredictionType::DownLeft,
		Intra4LumaPredictionType::DownRight,
		Intra4LumaPredictionType::VerticalRight,
		Intra4LumaPredictionType::HorizontalDown,
		Intra4LumaPredictionType::VerticalLeft,
		Intra4LumaPredictionType::HorizontalUp
	};


	int min_rdo_cost = std::numeric_limits<int>::max();
	for (auto prediction_type : prediction_types)
	{
		m_target_prediction_type = prediction_type;
		if (!DoFrontend())
			continue;

		int distortion = CalculateDistortion();
		if (distortion > min_rdo_cost)
			continue;

		int rate = CalculateRate();
		int rdo_cost = distortion + m_encoder_context->lambda_mode_fp * rate;
		if (rdo_cost < min_rdo_cost || (rdo_cost == min_rdo_cost && prediction_type == m_most_probable_prediction_type))
		{
			min_rdo_cost = rdo_cost;
			m_best_prediction_type = prediction_type;
		}
	}

	m_target_prediction_type = m_best_prediction_type;
	m_prediction_types[m_x_in_block + m_y_in_block * 4] = m_best_prediction_type;

	DoFrontend();

	m_reconstructed_data.SetBlock4x4(m_x_in_block, m_y_in_block, m_reconstructed_block_data);
}

bool Intra4LumaFlowNode::IsAllZero() const
{
	return m_is_all_zero;
}

Intra4LumaPredictionType Intra4LumaFlowNode::GetMostProbablePredictionType() const
{
	return m_most_probable_prediction_type;
}

Intra4LumaPredictionType Intra4LumaFlowNode::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

uint32_t Intra4LumaFlowNode::OutputCoefficients(std::shared_ptr<BytesData> bytes_data) const
{
	CavlcPreCoderLuma4x4 pre_coder;
	pre_coder.Code(m_residual_data);
	auto level_and_runs = pre_coder.GetLevelAndRuns();

	auto start_bits_count = bytes_data->GetBitsCount();

	CavlcNonCdcCoder coder(m_mb->GetAddress(), m_encoder_context->cavlc_context, bytes_data);
	coder.CodeNormalLuma(m_x_in_block + 4 * m_y_in_block, level_and_runs);

	auto finish_bits_count = bytes_data->GetBitsCount();
	return finish_bits_count - start_bits_count;
}

void Intra4LumaFlowNode::Init()
{
	m_original_block_data = m_mb->GetOriginalLumaBlockData4x4(m_x_in_block, m_y_in_block);
	m_predictor = std::make_unique<Intra4LumaPredictor>(m_mb, m_encoder_context, m_x_in_block, m_y_in_block, m_reconstructed_data, m_prediction_types);
	m_most_probable_prediction_type = m_predictor->GetMostProbablePredictionType();
}

bool Intra4LumaFlowNode::DoFrontend()
{
	if (!Predict())
		return false;

	Transform();
	Quantize();
	InverseQuantize();
	InverseTransform();
	Reconstruct();
	return true;
}

bool Intra4LumaFlowNode::Predict()
{
	m_predictor->Decide(m_target_prediction_type);
	auto prediction_type = m_predictor->GetPredictionType();
	if (prediction_type == Intra4LumaPredictionType::None)
		return false;

	m_diff_data = m_predictor->GetDiffData();
	return true;
}

void Intra4LumaFlowNode::Transform()
{
	m_diff_data = TransformUtil::DCT(m_diff_data);
}

void Intra4LumaFlowNode::Quantize()
{
	m_diff_data = QuantizeUtil::QuantizeNormal(m_encoder_context->qp, m_diff_data);
	m_residual_data = m_diff_data;
	m_is_all_zero = m_diff_data.AllEqual(0);
}

void Intra4LumaFlowNode::InverseQuantize()
{
	m_diff_data = QuantizeUtil::InverseQuantizeNormal(m_encoder_context->qp, m_diff_data);
}

void Intra4LumaFlowNode::InverseTransform()
{
	m_diff_data = TransformUtil::InverseDCT(m_diff_data);
}

void Intra4LumaFlowNode::Reconstruct()
{
	auto predicted_data = m_predictor->GetPredictedData();
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			auto residual_val = m_diff_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
			m_reconstructed_block_data.SetElement(x, y, val);
		}
	}
}

int Intra4LumaFlowNode::CalculateDistortion() const
{
	return CostUtil::CalculateSADDistortion(m_reconstructed_block_data, m_original_block_data);
}

int Intra4LumaFlowNode::CalculateRate() const
{
	int rate = (m_most_probable_prediction_type == GetPredictionType()) ? 1 : 4;
	std::shared_ptr<BytesData> bytes_data = std::make_shared<BytesData>();
	rate += OutputCoefficients(bytes_data);
	return rate;
}

__codec_end