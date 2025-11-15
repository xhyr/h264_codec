#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterP8x8LumaFlowNodeBase;

class InterP8x8LumaFlow : public InterLumaFlowBase
{
public:
	InterP8x8LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP8x8LumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputSubMBTypes(std::shared_ptr<BytesData> bytes_data) override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) override;

private:
	void Reconstruct();

	void Reconstruct(uint8_t segment_index);

	int64_t CalculateBlock8x8Distortion(uint32_t block_8x8);

private:
	std::vector<std::shared_ptr<InterP8x8LumaFlowNodeBase>> m_nodes;
};

__codec_end