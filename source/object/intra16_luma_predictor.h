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

class Intra16LumaPredictor
{
public:
	Intra16LumaPredictor(std::shared_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context);
	~Intra16LumaPredictor();

	void Decide();

	BlockData<16, 16> GetPredictedData() const;

	BlockData<16, 16, int32_t> GetDiffData() const;

	Intra16LumaPredictionType GetPredictionType() const;

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
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	bool m_left_available{ false };
	bool m_up_available{ false };
	std::vector<uint8_t> m_left_data;
	std::vector<uint8_t> m_up_data;
	uint8_t m_left_up_element;
	std::unordered_map<Intra16LumaPredictionType, BlockData<16, 16>> m_predicted_data_map;

	Intra16LumaPredictionType m_prediction_type;
	int m_cost{ -1 };
	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16, int32_t> m_diff_data;
};

__codec_end