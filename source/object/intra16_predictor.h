#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "mb_type.h"
#include "prediction_type.h"
#include "block_data.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class Intra16Predictor
{
public:
	Intra16Predictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context);
	~Intra16Predictor();

	Intra16PredictionType Decide();

	int GetCost() const;

	BlockData<16, 16> GetPredictedData() const;

	BlockData<16, 16, int> GetDiffData() const;

private:
	void ObtainLeftAndUpInfo();

	void CalculateAllPredictionData();

	void CalculateVerticalMode();

	void CalculateHorizontalMode();

	void CalculateDCMode();

	void CalculatePlaneMode();

	void DecideBySATD();

	void DecideBySAD();

private:
	std::weak_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	bool m_left_available;
	bool m_up_available;
	std::vector<uint8_t> m_left_data;
	std::vector<uint8_t> m_up_data;
	uint8_t m_left_up_element;
	std::unordered_map<Intra16PredictionType, BlockData<16, 16>> m_predicted_data_map;

	Intra16PredictionType m_prediction_type;
	int m_cost{ -1 };
	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16, int> m_diff_data;
};

__codec_end