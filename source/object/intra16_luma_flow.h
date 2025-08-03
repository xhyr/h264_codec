#pragma once

#include <memory>
#include <vector>

#include "intra_luma_flow_base.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class Intra16LumaPredictor;
class Intra16LumaQuantizer;

class Intra16LumaFlow : public IntraLumaFlowBase
{
public:
	Intra16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~Intra16LumaFlow();

	void Frontend() override;

	void Backend() override;

	Intra16LumaPredictionType GetPredictionType() const;

private:
	void Predict();

	void TransformAndQuantize();

	void InverseQuantizeAndTransform();

	void Reconstruct();

private:
	std::unique_ptr<Intra16LumaPredictor> m_predictor;
	std::unique_ptr<Intra16LumaQuantizer> m_quantizer;
	std::vector<BlockData<4, 4, int32_t>> m_diff_datas;
};

__codec_end