#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "block_data.h"
#include "prediction_type.h"
#include "common_types.h"
#include "cavlc_types.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class Intra8ChromaPredictor;
class ChromaQuantizer;
class BytesData;

class IntraChromaFlow
{
public:
	IntraChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~IntraChromaFlow();

	void SetTargetPredictionType(IntraChromaPredictionType prediction_type);

	void Frontend();

	IntraChromaPredictionType GetPredictionType() const;

	BlockData<8, 8> GetReconstructedData(PlaneType plane_type);

	uint8_t GetCBP() const;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> binary_block);

	int GetDistortion() const;

private:
	void Predict();

	void TransformAndQuantize(PlaneType plane_type);

	void InverseQuantizeAndTransform(PlaneType plane_type);

	void Reconstruct(PlaneType plane_type);

	void CalculateDistortion();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	std::unique_ptr<Intra8ChromaPredictor> m_predictor;
	std::unique_ptr<ChromaQuantizer> m_quantizer;

	std::unordered_map<PlaneType, BlockData<8, 8>> m_reconstructed_data_map;
	uint8_t m_cbp{ 0 };
	std::vector<BlockData<4, 4, int32_t>> m_diff_blocks;

	CavlcDataSource m_cavlc_data_source;

	IntraChromaPredictionType m_target_prediction_type{ IntraChromaPredictionType::None };
	int m_sse_distortion{ 0 };
};

__codec_end