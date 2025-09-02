#include "intra16_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "math_util.h"
#include "common_constant_values.h"
#include "transform_util.h"
#include "cost_util.h"

__codec_begin

Intra16LumaPredictor::Intra16LumaPredictor(std::shared_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context):
	m_mb(macroblock), m_encoder_context(encoder_context)
{
}

Intra16LumaPredictor::~Intra16LumaPredictor()
{
}

void Intra16LumaPredictor::Decide()
{
	ObtainLeftAndUpInfo();
	CalculateAllPredictionData();
	DecideBySATD();
}

BlockData<16, 16> Intra16LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

BlockData<16, 16, int32_t> Intra16LumaPredictor::GetDiffData() const
{
	return m_diff_data;
}

Intra16LumaPredictionType Intra16LumaPredictor::GetPredictionType() const
{
	return m_prediction_type;
}

void Intra16LumaPredictor::ObtainLeftAndUpInfo()
{
	m_mb->ObtainLeftAndUpEdge(m_left_data, m_up_data, m_left_up_element, PlaneType::Luma);
	m_left_available = !m_left_data.empty();
	m_up_available = !m_up_data.empty();
}

void Intra16LumaPredictor::CalculateAllPredictionData()
{
	CalculateVerticalMode();
	CalculateHorizontalMode();
	CalculateDCMode();
	CalculatePlaneMode();
}

void Intra16LumaPredictor::CalculateVerticalMode()
{
	if (!m_up_available)
		return;

	auto& block_data = m_predicted_data_map[Intra16LumaPredictionType::Vertical];
	for (uint32_t y = 0; y < 16; ++y)
		block_data.SetElementInRow(y, m_up_data);
}

void Intra16LumaPredictor::CalculateHorizontalMode()
{
	if (!m_left_available)
		return;

	auto& block_data = m_predicted_data_map[Intra16LumaPredictionType::Horizontal];
	for (uint32_t x = 0; x < 16; ++x)
		block_data.SetElementInColumn(x, std::vector<uint8_t>(m_left_data.begin(), m_left_data.end()));
}

void Intra16LumaPredictor::CalculateDCMode()
{
	uint8_t dc_value = CommonConstantValues::s_default_color_value;
	if (m_left_available && m_up_available)
	{
		auto sum = std::accumulate(m_up_data.begin(), m_up_data.end(), 0);
		sum += std::accumulate(m_left_data.begin(), m_left_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 5);
	}
	else if (m_left_available)
	{
		auto sum = std::accumulate(m_left_data.begin(), m_left_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 4);
	}
	else if (m_up_available)
	{
		auto sum = std::accumulate(m_up_data.begin(), m_up_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 4);
	}
	
	auto& block_data = m_predicted_data_map[Intra16LumaPredictionType::DC];
	block_data.SetElementAll(dc_value);
}

void Intra16LumaPredictor::CalculatePlaneMode()
{
	if (!m_left_available || !m_up_available)
		return;

	//calculate H
	int H = 8 * (m_up_data[15] - m_left_up_element);
	for (uint32_t x = 0; x < 7; ++x)
		H += (x + 1) * (m_up_data[8 + x] - m_up_data[6 - x]);

	//calculate V
	int V = 8 * (m_left_data[15] - m_left_up_element);
	for (uint32_t y = 0; y < 7; ++y)
		V += (y + 1) * (m_left_data[8 + y] - m_left_data[6 - y]);

	int a = 16 * (m_left_data[15] + m_up_data[15]);
	int b = MathUtil::RightShift(5 * H, 6);
	int c = MathUtil::RightShift(5 * V, 6);

	auto& block_data = m_predicted_data_map[Intra16LumaPredictionType::Plane];
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			int value = MathUtil::RightShift(a + b * (x - 7) + c * (y - 7), 5);
			value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			block_data.SetElement(x, y, value);
		}
	}
}

void Intra16LumaPredictor::DecideBySATD()
{
	auto mb = m_mb.lock();
	auto original_block_data = mb->GetOriginalLumaBlockData16x16();

	int min_satd = -1;
	Intra16LumaPredictionType best_prediction_type = Intra16LumaPredictionType::DC;

	std::vector<Intra16LumaPredictionType> prediction_types{ Intra16LumaPredictionType::Vertical, Intra16LumaPredictionType::Horizontal, Intra16LumaPredictionType::DC, Intra16LumaPredictionType::Plane };
	for (auto prediction_type : prediction_types)
	{
		if (m_predicted_data_map.find(prediction_type) == m_predicted_data_map.end())
			continue;

		const auto& predicted_data = m_predicted_data_map[prediction_type];
		auto diff_data = original_block_data - predicted_data;
		auto satd = CostUtil::CalculateSATDDistortion(diff_data);
		if (min_satd == -1 || satd < min_satd)
		{
			min_satd = satd;
			best_prediction_type = prediction_type;
			m_diff_data = diff_data;
			m_predicted_data = predicted_data;
		}
	}

	m_prediction_type = best_prediction_type;
	m_cost = min_satd;
}

void Intra16LumaPredictor::DecideBySAD()
{
	auto origin_block_data = m_mb->GetOriginalLumaBlockData16x16();

	int min_sad = -1;
	Intra16LumaPredictionType best_prediction_type = Intra16LumaPredictionType::DC;

	std::vector<Intra16LumaPredictionType> prediction_types{ Intra16LumaPredictionType::Vertical, Intra16LumaPredictionType::Horizontal, Intra16LumaPredictionType::DC, Intra16LumaPredictionType::Plane };
	for (auto prediction_type : prediction_types)
	{
		if (m_predicted_data_map.find(prediction_type) == m_predicted_data_map.end())
			continue;

		auto predicted_data = m_predicted_data_map[prediction_type];
		auto diff_data = origin_block_data - predicted_data;
		auto sad =  diff_data.GetAbstractSum();
		if (min_sad == -1 || sad < min_sad)
		{
			min_sad = sad;
			best_prediction_type = prediction_type;
		}
	}

	m_prediction_type = best_prediction_type;
	m_cost = min_sad;
}

__codec_end

