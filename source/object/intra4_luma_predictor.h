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
class BlockNeighbors;
struct NeighborEdgeData;

class Intra4LumaPredictor
{
public:
	Intra4LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t x_in_block, uint8_t y_in_block, const BlockData<16, 16>& reconstructed_data, const std::vector<Intra4LumaPredictionType>& prediction_tpyes);
	~Intra4LumaPredictor();

	void Decide(Intra4LumaPredictionType target_prediction_type);

	BlockData<4, 4> GetPredictedData() const;

	BlockData<4, 4, int32_t> GetDiffData() const;

	int GetCost() const;

	Intra4LumaPredictionType GetMostProbablePredictionType() const;

	Intra4LumaPredictionType GetPredictionType() const;

private:
	void Init();

	void CalculateMostProbablePredictionType();

	void CalculateAllPredictionData();

	void CalculateVerticalMode();

	void CalculateHorizontalMode();

	void CalculateDCMode();

	void CalculateDownLeftMode();

	void CalculateDownRightMode();

	void CalculateVerticalRightMode();

	void CalculateHorizontalDownMode();

	void CalculateVerticalLeftMode();

	void CalculateHorizontalUpMode();

	void DecideByRDO(uint32_t x_in_block, uint32_t y_in_block);

	void GenerateAllowedPredictionTypes(Intra4LumaPredictionType target_prediction_type);

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_x_in_block;
	uint8_t m_y_in_block;

	std::unique_ptr<BlockNeighbors> m_block_neighbors;
	std::shared_ptr<NeighborEdgeData> m_edge_data;

	std::unordered_map<Intra4LumaPredictionType, BlockData<4, 4>> m_predicted_data_map;

	Intra4LumaPredictionType m_most_probable_prediction_type;
	Intra4LumaPredictionType m_prediction_type{ Intra4LumaPredictionType::None };
	BlockData<4, 4> m_predicted_data;
	BlockData<4, 4, int32_t> m_diff_data;
	const BlockData<16, 16>& m_reconstructed_data;
	const std::vector<Intra4LumaPredictionType> m_prediction_types;
	int m_cost;
	std::unordered_map<Intra4LumaPredictionType, bool> m_allowed_prediction_type;
};

__codec_end