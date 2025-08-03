#include "intra4_luma_flow_node.h"

#include "intra4_luma_predictor.h"
#include "transform_util.h"
#include "macroblock.h"
#include "quantize_util.h"
#include "math_util.h"
#include "reconstruct_util.h"
#include "cavlc_coder_luma_4x4.h"

__codec_begin

Intra4LumaFlowNode::Intra4LumaFlowNode(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, BlockData<16, 16>& reconstructed_data, std::vector<Intra4LumaPredictionType>& prediction_types, uint8_t index_in_block8x8, uint8_t index_in_block4x4) :
	m_mb(mb), m_encoder_context(encoder_context), m_reconstructed_data(reconstructed_data), m_prediction_types(prediction_types),
	m_x_in_block((index_in_block8x8 % 2) * 2 + (index_in_block4x4 % 2)),
	m_y_in_block((index_in_block8x8 / 2) * 2 + (index_in_block4x4 / 2))
{
}

Intra4LumaFlowNode::~Intra4LumaFlowNode()
{
}

void Intra4LumaFlowNode::Frontend()
{
	Predict();
	Transform();
	Quantize();
	InverseQuantize();
	InverseTransform();
	Reconstruct();
}

int Intra4LumaFlowNode::GetCost() const
{
	return m_predictor->GetCost();
}

bool Intra4LumaFlowNode::IsAllZero() const
{
	return m_is_all_zero;
}

Intra4LumaPredictionType Intra4LumaFlowNode::GetMostProbablePredictionType() const
{
	return m_predictor->GetMostProbablePredictionType();
}

Intra4LumaPredictionType Intra4LumaFlowNode::GetPredictionType() const
{
	return m_predictor->GetPredictionType();
}

uint32_t Intra4LumaFlowNode::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	CavlcCoderLuma4x4 coder;
	coder.Code(m_diff_data);
	auto level_and_runs = coder.GetLevelAndRuns();

	return 0;
}

void Intra4LumaFlowNode::Predict()
{
	m_predictor = std::make_unique<Intra4LumaPredictor>(m_mb, m_encoder_context, m_x_in_block, m_y_in_block, m_reconstructed_data, m_prediction_types);
	m_predictor->Decide();
	auto prediction_type = m_predictor->GetPredictionType();
	m_prediction_types[m_x_in_block + m_y_in_block * 4] = prediction_type;
	m_diff_data = m_predictor->GetDiffData();
}

void Intra4LumaFlowNode::Transform()
{
	m_diff_data = TransformUtil::DCT(m_diff_data);
}

void Intra4LumaFlowNode::Quantize()
{
	auto qp = m_mb->GetQP();
	m_diff_data = QuantizeUtil::QuantizeNormal(qp, m_diff_data);
	m_is_all_zero &= m_diff_data.AllEqual(0);
}

void Intra4LumaFlowNode::InverseQuantize()
{
	auto qp = m_mb->GetQP();
	m_diff_data = QuantizeUtil::InverseQuantizeNormal(qp, m_diff_data);
}

void Intra4LumaFlowNode::InverseTransform()
{
	m_diff_data = TransformUtil::InverseDCT(m_diff_data);
}

void Intra4LumaFlowNode::Reconstruct()
{
	auto predicted_data = m_predictor->GetPredictedData();
	BlockData<4, 4> reconstructed_data;
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			auto residual_val = m_diff_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
			reconstructed_data.SetElement(x, y, val);
		}
	}
	m_reconstructed_data.SetBlock4x4(m_x_in_block, m_y_in_block, reconstructed_data);
}

__codec_end