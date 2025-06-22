#include "intra16_predictor.h"

#include <numeric>

#include "macroblock.h"
#include "encoder_context.h"

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

}

__codec_end

