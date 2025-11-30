#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "common_types.h"
#include "block_data.h"
#include "cavlc_types.h"
#include "mb_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class InterP2x2ChromaPredictor;

class ChromaQuantizer;

class InterChromaFlow
{
public:
	InterChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, bool skip);
	virtual ~InterChromaFlow();

	void Frontend();

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data);

	BlockData<8, 8> GetReconstructedData(PlaneType plane_type);

	uint8_t GetCBP() const;

	int GetSSEDistortion() const;

private:
	void Predict();

	void TransformAndQuantize(PlaneType plane_type);

	void InverseQuantizeAndTransform(PlaneType plane_type);

	void Reconstruct(PlaneType plane_type);

	void CalculateDistortion();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	bool m_skip;
	std::unique_ptr<InterP2x2ChromaPredictor> m_predictors[16];
	std::unique_ptr<ChromaQuantizer> m_quantizer;

	std::unordered_map<PlaneType, BlockData<8, 8>> m_reconstructed_data_map;
	std::unordered_map<PlaneType, BlockData<8, 8>> m_predicted_data_map;
	std::unordered_map<PlaneType, std::vector<BlockData<4, 4, int32_t>>> m_diff_datas_map;
	uint8_t m_cbp{ 0 };
	CavlcDataSource m_cavlc_data_source;
	int m_sse_distortion{ 0 };

};

__codec_end