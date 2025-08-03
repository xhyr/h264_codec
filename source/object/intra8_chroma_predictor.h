#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "mb_type.h"
#include "prediction_type.h"
#include "block_data.h"
#include "common_types.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class Intra8ChromaPredictor
{
public:
	Intra8ChromaPredictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context);
	~Intra8ChromaPredictor();

	void Decide();

	IntraChromaPredictionType GetPredictionType() const;

	int GetCost() const;

	BlockData<8, 8> GetPredictedData(PlaneType plane_type) const;

	BlockData<8, 8, int32_t> GetDiffData(PlaneType plane_type) const;

private:
	void ObtainLeftAndUpInfo(PlaneType plane_type);

	void CalculateAllPredictionData(PlaneType plane_type);

	void CalculateDCMode(PlaneType plane_type);

	void CalculateHorizontalMode(PlaneType plane_type);

	void CalculateVerticalMode(PlaneType plane_type);

	void CalculatePlaneMode(PlaneType plane_type);

	void DecideBySATD();

private:
	std::weak_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	bool m_left_available{ false };
	bool m_up_available{ false };
	std::vector<uint8_t> m_left_data;
	std::vector<uint8_t> m_up_data;
	uint8_t m_left_up_element;
	std::unordered_map<IntraChromaPredictionType, BlockData<8, 8>> m_predicted_data_map[2];

	IntraChromaPredictionType m_prediction_type;
	int m_cost{ -1 };
	BlockData<8, 8> m_predicted_data[2];
	BlockData<8, 8, int32_t> m_diff_data[2];
};

__codec_end