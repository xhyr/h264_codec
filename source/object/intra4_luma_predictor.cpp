#include "intra4_luma_predictor.h"

#include "block_neighbors.h"
#include "neighbor_edge_data.h"
#include "math_util.h"
#include "macroblock.h"
#include "cost_util.h"
#include "encoder_context.h"

__codec_begin

Intra4LumaPredictor::Intra4LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t x_in_block, uint8_t y_in_block, const BlockData<16, 16>& reconstructed_data, const std::vector<Intra4LumaPredictionType>& prediction_types) :
	m_mb(mb), m_encoder_context(encoder_context), m_x_in_block(x_in_block), m_y_in_block(y_in_block),
	m_reconstructed_data(reconstructed_data), m_prediction_types(prediction_types)
{
	Init();
}

Intra4LumaPredictor::~Intra4LumaPredictor()
{
}

int Intra4LumaPredictor::GetCost() const
{
	return m_cost;
}

Intra4LumaPredictionType Intra4LumaPredictor::GetMostProbablePredictionType() const
{
	return m_most_probable_prediction_type;
}

Intra4LumaPredictionType Intra4LumaPredictor::GetPredictionType() const
{
	return m_prediction_type;
}

void Intra4LumaPredictor::Decide(Intra4LumaPredictionType target_prediction_type)
{
	m_predicted_data_map.clear();

	GenerateAllowedPredictionTypes(target_prediction_type);
	CalculateAllPredictionData();
	Decide(m_x_in_block, m_y_in_block);
}

BlockData<4, 4> Intra4LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

BlockData<4, 4, int32_t> Intra4LumaPredictor::GetDiffData() const
{
	return m_diff_data;
}

void Intra4LumaPredictor::Init()
{
	m_block_neighbors = std::make_unique<BlockNeighbors>(m_mb, m_x_in_block, m_y_in_block, m_reconstructed_data, m_prediction_types);
	m_edge_data = m_block_neighbors->GetEdgeData();

	CalculateMostProbablePredictionType();
}

void Intra4LumaPredictor::CalculateMostProbablePredictionType()
{
	bool dc_pred_mode_predicted_flag = !m_block_neighbors->IsLeftAvailable() || !m_block_neighbors->IsUpAvailable();
	
	Intra4LumaPredictionType pred_mode_a, pred_mode_b;
	if (dc_pred_mode_predicted_flag == 1 || m_block_neighbors->GetLeftMBType() != MBType::I4)
		pred_mode_a = Intra4LumaPredictionType::DC;

	if (dc_pred_mode_predicted_flag == 1 || m_block_neighbors->GetUpMBType() != MBType::I4)
		pred_mode_b = Intra4LumaPredictionType::DC;

	if (dc_pred_mode_predicted_flag == 0 && m_block_neighbors->GetLeftMBType() == MBType::I4)
		pred_mode_a = m_block_neighbors->GetLeftPredictionType();
		
	if (dc_pred_mode_predicted_flag == 0 && m_block_neighbors->GetUpMBType() == MBType::I4)
		pred_mode_b = m_block_neighbors->GetUpPredictionType();

	m_most_probable_prediction_type = std::min(pred_mode_a, pred_mode_b);
}

void Intra4LumaPredictor::CalculateAllPredictionData()
{
	CalculateVerticalMode();
	CalculateHorizontalMode();
	CalculateDCMode();
	CalculateDownLeftMode();
	CalculateDownRightMode();
	CalculateVerticalLeftMode();
	CalculateHorizontalDownMode();
	CalculateVerticalRightMode();
	CalculateHorizontalUpMode();
}

void Intra4LumaPredictor::CalculateVerticalMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::Vertical])
		return;

	if (!m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::Vertical];
	for (uint32_t y = 0; y < 4; ++y)
	{
		block_data.SetElement(0, y, m_edge_data->a);
		block_data.SetElement(1, y, m_edge_data->b);
		block_data.SetElement(2, y, m_edge_data->c);
		block_data.SetElement(3, y, m_edge_data->d);
	}
}

void Intra4LumaPredictor::CalculateHorizontalMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::Horizontal])
		return;

	if (!m_block_neighbors->IsLeftAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::Horizontal];
	for (uint32_t x = 0; x < 4; ++x)
	{
		block_data.SetElement(x, 0, m_edge_data->i);
		block_data.SetElement(x, 1, m_edge_data->j);
		block_data.SetElement(x, 2, m_edge_data->k);
		block_data.SetElement(x, 3, m_edge_data->l);
	}
}

void Intra4LumaPredictor::CalculateDCMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::DC])
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::DC];
	if (m_block_neighbors->IsLeftAvailable() && m_block_neighbors->IsUpAvailable())
	{
		auto sum = m_edge_data->a + m_edge_data->b + m_edge_data->c + m_edge_data->d + m_edge_data->i + m_edge_data->j + m_edge_data->k + m_edge_data->l;
		auto val = MathUtil::RightShift(sum, 3);
		block_data.SetElementAll(val);
	}
	else if (m_block_neighbors->IsLeftAvailable())
	{
		auto sum = m_edge_data->i + m_edge_data->j + m_edge_data->k + m_edge_data->l;
		auto val = MathUtil::RightShift(sum, 2);
		block_data.SetElementAll(val);
	}
	else if (m_block_neighbors->IsUpAvailable())
	{
		auto sum = m_edge_data->a + m_edge_data->b + m_edge_data->c + m_edge_data->d;
		auto val = MathUtil::RightShift(sum, 2);
		block_data.SetElementAll(val);
	}
	else block_data.SetElementAll(CommonConstantValues::s_default_color_value);
}

void Intra4LumaPredictor::CalculateDownLeftMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::DownLeft])
		return;

	if (!m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::DownLeft];
	auto val = MathUtil::RightShift(m_edge_data->g + 3 * m_edge_data->h, 2);
	block_data.SetElement(3, 3, val);

	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			if (x == 3 && y == 3)
				continue;

			auto val = MathUtil::RightShift(m_edge_data->GetElement(x + y, -1) + 2 * m_edge_data->GetElement(x + y + 1, -1) + m_edge_data->GetElement(x + y + 2, -1), 2);
			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::CalculateDownRightMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::DownRight])
		return;

	if (!m_block_neighbors->IsLeftAvailable() || !m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::DownRight];
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = 0;
			if (x > y)
				val = MathUtil::RightShift(m_edge_data->GetElement(x - y - 2, -1) + 2 * m_edge_data->GetElement(x - y - 1, -1) + m_edge_data->GetElement(x - y, -1), 2);
			else if (x < y)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y - x - 2) + 2 * m_edge_data->GetElement(-1, y - x - 1) + m_edge_data->GetElement(-1, y - x), 2);
			else
				val = MathUtil::RightShift(m_edge_data->GetElement(0, -1) + 2 * m_edge_data->GetElement(-1, -1) + m_edge_data->GetElement(-1, 0), 2);

			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::CalculateVerticalRightMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::VerticalRight])
		return;

	if (!m_block_neighbors->IsLeftAvailable() || !m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::VerticalRight];
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = 0;
			int z_vr = 2 * x - y;
			if (z_vr == 0 || z_vr == 2 || z_vr == 4 || z_vr == 6)
				val = MathUtil::RightShift(m_edge_data->GetElement(x - (y >> 1) - 1, -1) + m_edge_data->GetElement(x - (y >> 1), -1), 1);
			else if (z_vr == 1 || z_vr == 3 || z_vr == 5)
				val = MathUtil::RightShift(m_edge_data->GetElement(x - (y >> 1) - 2, -1) + 2 * m_edge_data->GetElement(x - (y >> 1) - 1, -1) + m_edge_data->GetElement(x - (y >> 1), -1), 2);
			else if (z_vr == -1)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, 0) + 2 * m_edge_data->GetElement(-1, -1) + m_edge_data->GetElement(0, -1), 2);
			else
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y - 1) + 2 * m_edge_data->GetElement(-1, y - 2) + m_edge_data->GetElement(-1, y - 3), 2);

			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::CalculateHorizontalDownMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalDown])
		return;

	if (!m_block_neighbors->IsLeftAvailable() || !m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::HorizontalDown];
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = 0;
			int z_hd = 2 * y - x;

			if (z_hd == 0 || z_hd == 2 || z_hd == 4 || z_hd == 6)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y - (x >> 1) - 1) + m_edge_data->GetElement(-1, y - (x >> 1)), 1);
			else if (z_hd == 1 || z_hd == 3 || z_hd == 5)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y - (x >> 1) - 2) + 2 * m_edge_data->GetElement(-1, y - (x >> 1) - 1) + m_edge_data->GetElement(-1, y - (x >> 1)), 2);
			else if (z_hd == -1)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, 0) + 2 * m_edge_data->GetElement(-1, -1) + m_edge_data->GetElement(0, -1), 2);
			else 
				val = MathUtil::RightShift(m_edge_data->GetElement(x - 1, -1) + 2 * m_edge_data->GetElement(x - 2, -1) + m_edge_data->GetElement(x - 3, -1), 2);

			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::CalculateVerticalLeftMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::VerticalLeft])
		return;

	if (!m_block_neighbors->IsUpAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::VerticalLeft];
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = 0;
			if (y == 0 || y == 2)
				val = MathUtil::RightShift(m_edge_data->GetElement(x + (y >> 1), -1) + m_edge_data->GetElement(x + (y >> 1) + 1, -1), 1);
			else
				val = MathUtil::RightShift(m_edge_data->GetElement(x + (y >> 1), -1) + 2 * m_edge_data->GetElement(x + (y >> 1) + 1, -1) + m_edge_data->GetElement(x + (y >> 1) + 2, -1), 2);

			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::CalculateHorizontalUpMode()
{
	if (!m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalUp])
		return;

	if (!m_block_neighbors->IsLeftAvailable())
		return;

	auto& block_data = m_predicted_data_map[Intra4LumaPredictionType::HorizontalUp];
	for (uint32_t y = 0; y < 4; ++y)
	{
		for (uint32_t x = 0; x < 4; ++x)
		{
			auto val = 0;
			int z_hu = x + 2 * y;
			if (z_hu == 0 || z_hu == 2 || z_hu == 4)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y + (x >> 1)) + m_edge_data->GetElement(-1, y + (x >> 1) + 1), 1);
			else if (z_hu == 1 || z_hu == 3)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, y + (x >> 1)) + 2 * m_edge_data->GetElement(-1, y + (x >> 1) + 1) + m_edge_data->GetElement(-1, y + (x >> 1) + 2), 2);
			else if (z_hu == 5)
				val = MathUtil::RightShift(m_edge_data->GetElement(-1, 2) + 3 * m_edge_data->GetElement(-1, 3), 2);
			else
				val = m_edge_data->GetElement(-1, 3);
			block_data.SetElement(x, y, val);
		}
	}
}

void Intra4LumaPredictor::Decide(uint32_t x_in_block, uint32_t y_in_block)
{
	if (m_predicted_data_map.empty())
		return;

	auto original_block_data = m_mb->GetOriginalLumaBlockData4x4(x_in_block, y_in_block);

	int min_cost = -1;
	Intra4LumaPredictionType best_prediction_type = Intra4LumaPredictionType::DC;
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
	for (auto prediction_type : prediction_types)
	{
		if (m_predicted_data_map.find(prediction_type) == m_predicted_data_map.end())
			continue;

		const auto& predicted_data = m_predicted_data_map[prediction_type];
		auto diff_data = original_block_data - predicted_data;
		
		best_prediction_type = prediction_type;
		m_predicted_data = predicted_data;
		m_diff_data = diff_data;
	}

	m_prediction_type = best_prediction_type;
}

void Intra4LumaPredictor::GenerateAllowedPredictionTypes(Intra4LumaPredictionType target_prediction_type)
{
	if (target_prediction_type != Intra4LumaPredictionType::None)
	{
		m_allowed_prediction_type[Intra4LumaPredictionType::Vertical] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::Horizontal] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::DC] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::DownLeft] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::DownRight] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::VerticalRight] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalDown] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::VerticalLeft] = false;
		m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalUp] = false;
		m_allowed_prediction_type[target_prediction_type] = true;
	}
	else
	{
		m_allowed_prediction_type[Intra4LumaPredictionType::Vertical] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::Horizontal] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::DC] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::DownLeft] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::DownRight] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::VerticalRight] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalDown] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::VerticalLeft] = true;
		m_allowed_prediction_type[Intra4LumaPredictionType::HorizontalUp] = true;
	}
}

__codec_end
