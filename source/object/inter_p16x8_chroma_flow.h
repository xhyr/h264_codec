#pragma once

#include "inter_chroma_flow_base.h"

__codec_begin

class InterP16x8ChromaPredictor;

class InterP16x8ChromaFlow : public InterChromaFlowBase
{
public:
	InterP16x8ChromaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x8ChromaFlow();

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
	std::unique_ptr<InterP16x8ChromaPredictor> m_predictors[2];
};



__codec_end