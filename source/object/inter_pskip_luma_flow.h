#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterPSkipLumaPredictor;

class InterPSkipLumaFlow : public InterLumaFlowBase
{
public:
	InterPSkipLumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterPSkipLumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) const override;

private:
	void Predict();

	void Reconstruct();

private:
	std::unique_ptr<InterPSkipLumaPredictor> m_predictor;
};

__codec_end