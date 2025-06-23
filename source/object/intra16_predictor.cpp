#include "intra16_predictor.h"

#include <numeric>

#include "macroblock.h"
#include "encoder_context.h"
#include "math_util.h"

__codec_begin

Intra16Predictor::Intra16Predictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context):
	m_mb(macroblock), m_encoder_context(encoder_context)
{
}

Intra16Predictor::~Intra16Predictor()
{
}

Intra16PredictionType Intra16Predictor::Decide()
{
	ObtainLeftAndUpInfo();
	CalculateAllPredictionData();

	return m_prediction_type;
}

void Intra16Predictor::ObtainLeftAndUpInfo()
{
	auto mb = m_mb.lock();

	mb->ObtainLeftAndUpData(m_left_data, m_up_data);
	m_left_available = !m_left_data.empty();
	m_up_available = !m_up_data.empty();
}

void Intra16Predictor::CalculateAllPredictionData()
{
	CalculateVerticalMode();
	CalculateHorizontalMode();
	CalculateDCMode();
	CalculatePlaneMode();
}

void Intra16Predictor::CalculateVerticalMode()
{
	if (!m_up_available)
		return;

	auto& block_data = m_predicted_data[Intra16PredictionType::Vertical];
	for (uint32_t y = 0; y < MB_HEIGHT; ++y)
		block_data.SetElementInRow(y, m_up_data);
}

void Intra16Predictor::CalculateHorizontalMode()
{
	if (!m_left_available)
		return;

	auto& block_data = m_predicted_data[Intra16PredictionType::Horizontal];
	for (uint32_t x = 0; x < MB_WIDTH; ++x)
		block_data.SetElementInColumn(x, std::vector<uint8_t>(std::next(m_left_data.begin()), m_left_data.end()));
}

void Intra16Predictor::CalculateDCMode()
{
	uint8_t dc_value = DEFAULT_COLOR_VALUE;
	if (m_left_available && m_up_available)
	{
		auto sum = std::accumulate(m_up_data.begin(), m_up_data.end(), 0);
		sum += std::accumulate(std::next(m_left_data.begin()), m_left_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 5);
	}
	else if (m_left_available)
	{
		auto sum = std::accumulate(std::next(m_left_data.begin()), m_left_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 4);
	}
	else if (m_up_available)
	{
		auto sum = std::accumulate(m_up_data.begin(), m_up_data.end(), 0);
		dc_value = MathUtil::RightShift(sum, 4);
	}
	
	auto& block_data = m_predicted_data[Intra16PredictionType::DC];
	block_data.SetElementAll(dc_value);
}

void Intra16Predictor::CalculatePlaneMode()
{
	if (!m_left_available || !m_up_available)
		return;

	//calculate H
	int H = 0;
	for (uint32_t x = 0; x < 8; ++x)
		H += (x + 1) * (m_up_data[8 + x] - m_up_data[6 - x]);

	//calculate V
	int V = 0;
	for (uint32_t y = 0; y < 8; ++y)
		V += (y + 1) * (m_left_data[1 + 8 + y] - m_left_data[1 + 6 - y]);

	int a = 16 * (m_left_data[16] + m_up_data[15]);
	int b = MathUtil::RightShift(5 * H, 6);
	int c = MathUtil::RightShift(5 * V, 6);

	auto& block_data = m_predicted_data[Intra16PredictionType::Plane];
	for (uint32_t y = 0; y < MB_HEIGHT; ++y)
	{
		for (uint32_t x = 0; x < MB_WIDTH; ++x)
		{
			int value = MathUtil::RightShift(a + b * (x - 7) + c * (y - 7), 5);
			value = MathUtil::Clamp(value, MIN_COLOR_VALUE, MAX_COLOR_VALUE);
			block_data.SetElement(x, y, value);
		}
	}
}

__codec_end

