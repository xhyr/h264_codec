#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterP16x8LumaPredictor;

class InterP16x8LumaFlow : public InterLumaFlowBase
{
public:
	InterP16x8LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x8LumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) const override;

private:
	void Predict();

	void Reconstruct();

private:
	std::unique_ptr<InterP16x8LumaPredictor> m_predictors[2];
};

__codec_end