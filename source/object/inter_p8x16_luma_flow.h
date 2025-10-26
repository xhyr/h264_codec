#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterP8x16LumaPredictor;

class InterP8x16LumaFlow : public InterLumaFlowBase
{
public:
	InterP8x16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP8x16LumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) override;

private:
	void Predict();

	void Reconstruct();

private:
	std::unique_ptr<InterP8x16LumaPredictor> m_predictors[2];
};

__codec_end