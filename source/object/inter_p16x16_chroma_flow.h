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
class InterP16x16ChromaPredictor;
class ChromaQuantizer;
class BytesData;

class InterP16x16ChromaFlow
{
public:
	InterP16x16ChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x16ChromaFlow();

	void Frontend();

	BlockData<8, 8> GetReconstructedData(PlaneType plane_type);

	uint8_t GetCBP() const;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data);

	int GetDistortion();

private:
	void Predict();

	void TransformAndQuantize(PlaneType plane_type);

	void InverseQuantizeAndTransform(PlaneType plane_type);

	void Reconstruct(PlaneType plane_type);

	void CalculateDistortion();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	std::unique_ptr<InterP16x16ChromaPredictor> m_predictor;
	std::unique_ptr<ChromaQuantizer> m_quantizer;

	std::unordered_map<PlaneType, BlockData<8, 8>> m_reconstructed_data_map;
	uint8_t m_cbp{ 0 };
	std::vector<BlockData<4, 4, int32_t>> m_diff_blocks;
	CavlcDataSource m_cavlc_data_source;
	int m_distortion{ 0 };
};

__codec_end