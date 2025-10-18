#pragma once

#include "inter_chroma_flow_base.h"

__codec_begin

class InterP8x16ChromaPredictor;

class InterP8x16ChromaFlow : public InterChromaFlowBase
{
public:
	InterP8x16ChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP8x16ChromaFlow();

	void Frontend() override;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) override;

private:
	void Predict();

	void TransformAndQuantize(PlaneType plane_type);

	void InverseQuantizeAndTransform(PlaneType plane_type);

	void Reconstruct(PlaneType plane_type);

	void CalculateDistortion();

	BlockData<8, 8, int32_t> GetDiffData(PlaneType plane_type) const;

	BlockData<8, 8> GetPredictedData(PlaneType plane_type) const;

protected:
	std::unique_ptr<InterP8x16ChromaPredictor> m_predictors[2];
};



__codec_end