#pragma once

#include <memory>

#include "inter_p8x8_luma_flow_node_base.h"

__codec_begin

class InterP8x8LumaPredictor;

class InterP8x8LumaFlow8x8Node : public InterP8x8LumaFlowNodeBase
{
public:
	InterP8x8LumaFlow8x8Node(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	~InterP8x8LumaFlow8x8Node();

	void Predict() override;

	std::vector<MotionInfo> GetMotionInfos() const override;

	std::vector<MotionVector> GetMVDs() const override;

private:
	void Init();

private:
	std::unique_ptr<InterP8x8LumaPredictor> m_predictor;
};

__codec_end