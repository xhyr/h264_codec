#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterSubP8x8LumaFlow;

class InterP8x8LumaFlow : public InterLumaFlowBase
{
public:
	InterP8x8LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP8x8LumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) override;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) override;

private:
	

private:
	std::vector<std::shared_ptr<InterSubP8x8LumaFlow>> m_sub_flows;
	std::vector<BlockData<4, 4, int32_t>> m_diff_datas;
	BlockData<16, 16, int32_t> m_diff_data;
	std::vector<BlockData<4, 4, int32_t>> m_residual_datas;
	uint32_t m_coefficient_cost{ 0 };
};

__codec_end