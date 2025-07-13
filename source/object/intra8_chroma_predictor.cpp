#include "intra8_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "math_util.h"
#include "common_constant_values.h"
#include "transform_util.h"
#include "cost_util.h"

__codec_begin

Intra8ChromaPredictor::Intra8ChromaPredictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context):
	m_mb(macroblock), m_encoder_context(encoder_context)
{
}

Intra8ChromaPredictor::~Intra8ChromaPredictor()
{
}

IntraChromaPredictionType Intra8ChromaPredictor::Decide()
{
	std::vector<PlaneType> plane_types{ PlaneType::Cb, PlaneType::Cr };
	for (auto plane_type : plane_types)
	{
		ObtainLeftAndUpInfo(plane_type);
		CalculateAllPredictionData(plane_type);
	}
	DecideBySATD();
	return m_prediction_type;
}

int Intra8ChromaPredictor::GetCost() const
{
	return m_cost;
}

BlockData<8, 8> Intra8ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<8, 8, int32_t> Intra8ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

void Intra8ChromaPredictor::ObtainLeftAndUpInfo(PlaneType plane_type)
{
	auto mb = m_mb.lock();

	mb->ObtainLeftAndUpEdge(m_left_data, m_up_data, m_left_up_element, plane_type);
	m_left_available = !m_left_data.empty();
	m_up_available = !m_up_data.empty();
}

void Intra8ChromaPredictor::CalculateAllPredictionData(PlaneType plane_type)
{
	CalculateDCMode(plane_type);
	CalculateHorizontalMode(plane_type);
	CalculateVerticalMode(plane_type);
	CalculatePlaneMode(plane_type);
}

void Intra8ChromaPredictor::CalculateDCMode(PlaneType plane_type)
{
	auto& predicted_data = m_predicted_data_map[plane_type == PlaneType::Cb ? 0 : 1][IntraChromaPredictionType::DC];
	for (uint32_t y_in_block = 0; y_in_block < 2; ++y_in_block)
	{
		for (uint32_t x_in_block = 0; x_in_block < 2; ++x_in_block)
		{
			uint32_t block_index = y_in_block * 2 + x_in_block;
			uint8_t dc_value = CommonConstantValues::s_default_color_value;
			switch (block_index)
			{
			case 0:
			{
				if (!m_left_available && !m_up_available)
					break;

				int sum = 0; 
				if (m_left_available)
					sum = std::accumulate(m_left_data.begin(), std::next(m_left_data.begin(), 4), sum);
				if (m_up_available)
					sum = std::accumulate(m_up_data.begin(), std::next(m_up_data.begin(), 4), sum);
				int shift_bits = (m_left_available && m_up_available) ? 3 : 2;
				int value = MathUtil::RightShift(sum, shift_bits);
				dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			}
			break;
			case 1:
			{
				if (!m_left_available && !m_up_available)
					break;

				if (m_up_available)
				{
					int sum = std::accumulate(std::next(m_up_data.begin(), 4), m_up_data.end(), 0);
					int value = MathUtil::RightShift(sum, 2);
					dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				}
				else if (m_left_available)
				{
					int sum = std::accumulate(m_left_data.begin(), std::next(m_left_data.begin(), 4), 0);
					int value = MathUtil::RightShift(sum, 2);
					dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				}
			}
			break;
			case 2:
			{
				if (!m_left_available && !m_up_available)
					break;

				if (m_left_available)
				{
					int sum = std::accumulate(std::next(m_left_data.begin(), 4), m_left_data.end(), 0);
					int value = MathUtil::RightShift(sum, 2);
					dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				}
				else if (m_up_available)
				{
					int sum = std::accumulate(m_up_data.begin(), std::next(m_up_data.begin(), 4), 0);
					int value = MathUtil::RightShift(sum, 2);
					dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				}
			}
			break;
			case 3:
			{
				if (!m_left_available && !m_up_available)
					break;

				int sum = 0;
				if (m_left_available)
					sum = std::accumulate(std::next(m_left_data.begin(), 4), m_left_data.end(), sum);
				if (m_up_available)
					sum = std::accumulate(std::next(m_up_data.begin(), 4), m_up_data.end(), sum);
				int shift_bits = (m_left_available && m_up_available) ? 3 : 2;
				int value = MathUtil::RightShift(sum, shift_bits);
				dc_value = MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			}
			break;
			}
			
			//assign 
			for (uint32_t y = y_in_block * 4; y < y_in_block * 4 + 4; ++y)
				for (uint32_t x = x_in_block * 4; x < x_in_block * 4 + 4; ++x)
					predicted_data.SetElement(x, y, dc_value);
		}
	}
}

void Intra8ChromaPredictor::CalculateHorizontalMode(PlaneType plane_type)
{
	if (!m_left_available)
		return;

	auto& predicted_data = m_predicted_data_map[plane_type == PlaneType::Cb ? 0 : 1][IntraChromaPredictionType::Horizontal];
	for (uint32_t y = 0; y < 8; ++y)
		predicted_data.SetElementInRow(y, std::vector<uint8_t>(8, m_left_data[y]));
}

void Intra8ChromaPredictor::CalculateVerticalMode(PlaneType plane_type)
{
	if (!m_up_available)
		return;

	auto& predicted_data = m_predicted_data_map[plane_type == PlaneType::Cb ? 0 : 1][IntraChromaPredictionType::Vertical];
	for (uint32_t x = 0; x < 8; ++x)
		predicted_data.SetElementInColumn(x, std::vector<uint8_t>(8, m_up_data[x]));
}

void Intra8ChromaPredictor::CalculatePlaneMode(PlaneType plane_type)
{
	if (!m_left_available || !m_up_available)
		return;

	auto& predicted_data = m_predicted_data_map[plane_type == PlaneType::Cb ? 0 : 1][IntraChromaPredictionType::Plane];
	int x_cf = 0;
	int y_cf = 0;

	int H = (3 + 1) * (m_up_data[7] - m_left_up_element);
	for (int x = 0; x < 3; ++x)
		H += (x + 1) * (m_up_data[4 + x] - m_up_data[2 - x]);

	int V = (3 + 1) * (m_left_data[7] - m_left_up_element);
	for (int y = 0; y < 3; ++y)
		V += (y + 1) * (m_left_data[4 + y] - m_left_data[2 - y]);

	int a = 16 * (m_left_data[7] + m_up_data[7]);
	int b = MathUtil::RightShift(34 * H, 6);
	int c = MathUtil::RightShift(34 * V, 6);

	for (uint32_t y = 0; y < 8; ++y)
	{
		for (uint32_t x = 0; x < 8; ++x)
		{
			int value = MathUtil::RightShift(a + b * (x - 3 - x_cf) + c * (y - 3 - y_cf), 5);
			predicted_data.SetElement(x, y, MathUtil::Clamp<int>(value, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value));
		}
	}
}

void Intra8ChromaPredictor::DecideBySATD()
{
	auto mb = m_mb.lock();

	std::unordered_map<PlaneType, BlockData<8, 8>> origial_block_data_map;
	std::vector<PlaneType> plane_types{ PlaneType::Cb, PlaneType::Cr };
	for(auto plane_type : plane_types)
		origial_block_data_map[plane_type] = mb->GetOriginalChromaBlockData(plane_type);

	int min_satd = -1;
	IntraChromaPredictionType best_prediction_type = IntraChromaPredictionType::DC;
	std::vector<IntraChromaPredictionType> prediction_types{ IntraChromaPredictionType::DC, IntraChromaPredictionType::Horizontal, IntraChromaPredictionType::Vertical, IntraChromaPredictionType::Plane };
	for (auto prediction_type : prediction_types)
	{
		if (m_predicted_data_map[0].find(prediction_type) == m_predicted_data_map[0].end())
			continue;

		int satd = 0;
		BlockData<8, 8, int32_t> uv_diff_data[2];
		for (auto plane_type : plane_types)
		{
			const auto& original_block_data = origial_block_data_map[plane_type];
			const auto& predicted_data = m_predicted_data_map[plane_type == PlaneType::Cb ? 0 : 1][prediction_type];
			auto diff_data = original_block_data - predicted_data;
			uv_diff_data[plane_type == PlaneType::Cb ? 0 : 1] = diff_data;
			satd += CostUtil::CalculateSATD(diff_data);
		}

		if (min_satd == -1 || satd < min_satd)
		{
			min_satd = satd;
			best_prediction_type = prediction_type;
			m_diff_data[0] = uv_diff_data[0];
			m_diff_data[1] = uv_diff_data[1];
		}
	}

	m_prediction_type = best_prediction_type;
	m_predicted_data[0] = m_predicted_data_map[0][best_prediction_type];
	m_predicted_data[1] = m_predicted_data_map[1][best_prediction_type];
	m_cost = min_satd;
}

__codec_end