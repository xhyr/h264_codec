#pragma once

#include <memory>

#include "inter_p8x8_luma_flow_node_base.h"

__codec_begin

class InterP4x8LumaPredictor;

class InterP8x8LumaFlow4x8Node : public InterP8x8LumaFlowNodeBase
{
public:
	InterP8x8LumaFlow4x8Node(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	~InterP8x8LumaFlow4x8Node();

	void Predict() override;

	void FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const override;

	void UpdateMotionInfo() override;

	uint32_t OutputMotionInfos(std::shared_ptr<BytesData> bytes_data) const;

private:
	void Init();

private:
	std::unique_ptr<InterP4x8LumaPredictor> m_predictors[2];
};

__codec_end