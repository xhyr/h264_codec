#pragma once

#include <memory>
#include <vector>

#include "block_data.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class Intra4LumaPredictor;
class BytesData;

class Intra4LumaFlowNode
{
public:
	Intra4LumaFlowNode(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, BlockData<16, 16>& reconstructed_data, std::vector<Intra4LumaPredictionType>& prediction_types, uint8_t index_in_block8x8, uint8_t index_in_block4x4);
	~Intra4LumaFlowNode();

	void SetTargetPredictionType(Intra4LumaPredictionType prediction_type);

	void Frontend();

	bool IsAllZero() const;

	Intra4LumaPredictionType GetMostProbablePredictionType() const;

	Intra4LumaPredictionType GetPredictionType() const;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) const;

private:
	bool DoFrontend();

	bool Predict();

	void Transform();

	void Quantize();

	void InverseQuantize();

	void InverseTransform();

	void Reconstruct();

	int CalculateDistortion() const;

	int CalculateRate() const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	BlockData<16, 16>& m_reconstructed_data;
	std::vector<Intra4LumaPredictionType>& m_prediction_types;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;

	BlockData<4, 4> m_original_block_data;
	std::unique_ptr<Intra4LumaPredictor> m_predictor;
	BlockData<4, 4, int32_t> m_diff_data;
	BlockData<4, 4, int32_t> m_residual_data;
	bool m_is_all_zero{ true };

	Intra4LumaPredictionType m_target_prediction_type{ Intra4LumaPredictionType::None };
	Intra4LumaPredictionType m_best_prediction_type{ Intra4LumaPredictionType::None };
};

__codec_end